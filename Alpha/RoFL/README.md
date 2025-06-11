# RoFL

This folder contains the configurations and script used for RoFL benchmarking.

1. Copy all files in this folder to the RoFL folder (commit `c1a0c13af4fb15b5c2ac9e0ca31d8623c0b0ee9b`).
2. Install RoFL and FL-analysis according to the documentation.
3. Set dataset manually in `rofl_train_client/trainservice/analysis_wrapper/data_loader.py` line 9-10.
4. Run specific benchmarking `bash run.sh`.

Note that the original RoFL and FL-analysis repos are poorly documented about the dataset, especially the Shakespeare, which might need to be downloaded and parsed separately.