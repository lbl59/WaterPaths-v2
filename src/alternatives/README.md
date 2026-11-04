# alternatives/ — actions_rdm one-to-one realization mapping

This folder contains **alternative, non-committed** copies of source files, modified
to change how `actions_rdm` is loaded and threaded through the simulation. **No
original file under `src/` was modified** — every file here is a standalone copy
placed at the same relative path it has in `src/`, so a reviewer can diff each one
against the original directly, e.g.:

```
diff src/main.cpp src/alternatives/main.cpp
diff src/SystemComponents/Utility/Utility.cpp src/alternatives/SystemComponents/Utility/Utility.cpp
```

## Motivation

`actions_rdm` is the RDM (Robust Decision Making) perturbation matrix for
infrastructure-trigger actions. Before this change, when a single RDM sample number
was passed on the command line (`-R <rdm_no>`, i.e. `rdm_no != NON_INITIALIZED` in
`main.cpp`), the code read **one row** from the actions RDM file
(`actions_rdm_file[rdm_no]`) and **duplicated that same row across every
realization**:

```cpp
auto actions_rdm_row = Utils::parse2DCsvFile(system_io + actions_rdm_file)[rdm_no];
actions_rdm = std::vector<vector<double>>(n_realizations, actions_rdm_row);
```

This is inconsistent with the `rdm_no == NON_INITIALIZED` branch, which already read
the **entire file** and let each realization use its own row
(`actions_rdm = Utils::parse2DCsvFile(...)`). It's also inconsistent with how
`utilities_rdm`, `water_sources_rdm`, and `policies_rdm` are perturbed in RDM mode
in the same branch of the code (see `PaperTestProblem.cpp` and the RDM factor files),
which do vary across the ensemble in ways the actions matrix did not.

Separately, row selection for `actions_rdm` happened in `Simulation.cpp`
(`actions_rdm.at(realization)`), one layer removed from where the analogous
per-realization row indexing for demand/financial data already happens
(`Utility::setRealization`). This change moves that indexing into
`Utility::setRealization` itself, alongside an explicit bounds check, so the
realization-to-row mapping is validated at the point where it is actually consumed
instead of relying on an unchecked `.at()` call three layers upstream.

## Changes, file by file

### `main.cpp`
- In the `rdm_no != NON_INITIALIZED` branch (previously lines ~292–298): instead of
  extracting a single row and duplicating it `n_realizations` times, the full
  `actions_rdm_file` is now parsed and assigned directly to `actions_rdm`, matching
  the behavior already used in the `else` branch.
- Added an explicit check in **both** branches: if `n_realizations` exceeds the
  number of rows in `actions_rdm`, a `length_error` is thrown before
  `problem.setRDMReevaluation(...)` is ever called. This mirrors the existing
  `utilities_rdm.size()` check already present in both branches.

### `SystemComponents/Utility/Utility.h` / `Utility.cpp`
- `Utility::setRealization` now takes `vector<vector<double>> &actions_rdm` (the
  full realization-by-row matrix) instead of `vector<double> &actions_rdm` (an
  already-selected single row).
- Inside the function, the realization index `r` is used to select the matching row
  (`actions_rdm.at(r).at(action_rdm_id)`), and — before that lookup — a bounds
  check is added:

  ```cpp
  if (r >= actions_rdm.size()) {
      throw std::out_of_range(...);  // descriptive message with r and actions_rdm.size()
  }
  ```

  This is the one-to-one realization/row matching and mismatch check requested:
  it fails loudly with a clear message instead of silently reading out of bounds
  (undefined behavior) or wrapping around.

### `ContinuityModels/Base/ContinuityModel.h` / `ContinuityModel.cpp`
- The `actions_rdm` member, constructor parameter, and `setRealization` parameter
  all changed from `vector<double>` to `vector<vector<double>>`.
- `ContinuityModel::setRealization` now passes the **whole** `actions_rdm` matrix
  (plus `realization_id`) down to `Utility::setRealization`, instead of a
  pre-selected single row. Row selection now happens once, in `Utility::setRealization`
  (see above), rather than being done for every `ContinuityModel` upstream of it.
  `WaterSource::setRealization` and `MinEnvFlowControl::setRealization` are
  unaffected — they never took `actions_rdm`.

### `ContinuityModels/ContinuityModelRealization.h` / `ContinuityModelRealization.cpp`
- `actions_rdm` parameter changed to `vector<vector<double>>&` and is forwarded
  unchanged to the `ContinuityModel` base constructor.
- `DroughtMitigationPolicy::setRealization` was **not** changed (still takes a
  single-row `vector<double>&`), so the constructor now extracts the row for this
  realization explicitly before calling it: `actions_rdm.at(realization_id)`.

### `ContinuityModels/ContinuityModelROF.h` / `ContinuityModelROF.cpp`
- `actions_rdm` parameter changed to `vector<vector<double>>&` and forwarded
  unchanged to the `ContinuityModel` base constructor. No other logic in this file
  touches `actions_rdm`.

### `Simulation/Simulation.cpp`
- `Simulation::createContinuityModels` previously called
  `actions_rdm.at(realization)` when constructing both the
  `ContinuityModelRealization` and `ContinuityModelROF` objects for a realization.
  Both call sites now pass `actions_rdm` (the full matrix, already a
  `vector<vector<double>>&` member) instead, since row selection has moved
  downstream into `Utility::setRealization`. `utilities_rdm`, `water_sources_rdm`,
  and `policies_rdm` indexing is unchanged — only `actions_rdm` handling moved.

## Files intentionally left unchanged

- `Problem/Base/Problem.cpp` / `.h` and `Problem/PaperTestProblem.cpp` — these
  already store/forward `actions_rdm` as `vector<vector<double>>` and needed no
  changes.
- `DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h` (and its
  subclasses, e.g. `Transfers`, `Restrictions`) — `setRealization` there still
  takes a single-row `vector<double>&`; `ContinuityModelRealization.cpp` supplies
  that row via `actions_rdm.at(realization_id)`.

## Not yet done

- These files have **not** been compiled against the full build (HDF5/Arrow/OpenMP
  toolchain) from this session — only manually traced for type/signature
  consistency end to end. Before adopting, run `make gcc` (or `make borg`) with
  these files swapped in for their originals and confirm a clean build, then
  re-run a known solution/RDM combination and compare `Objectives_*.csv` output
  against the current baseline.
- If `actions_rdm_file` genuinely should have exactly `n_realizations` rows per RDM
  sample (rather than one row per RDM sample reused across realizations, which was
  the old behavior), confirm the actions RDM CSV files under `TestFiles/` are
  already laid out that way — this change assumes the file format matches the
  `utilities_rdm`/`water_sources_rdm`/`policies_rdm` files (rows = realizations),
  consistent with how the `rdm_no == NON_INITIALIZED` branch already read it.
