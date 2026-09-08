python convert_audio_weights.py <params.h> <output.bin>

example:

cd /mnt/d/software/IP7AI/ipro_sdk/apps/platform/ipro7_demo/demo/ai/tools
python convert_audio_weights.py params.h audio_weights.bin

If your dimensions are different:

If your new model dimensions are not EMB_SIZE = 48 and POOL_DIM = 5120, 
you will need to modify lines 81–82 in the script.

  EMB_SIZE = 48    #  embedding size
  POOL_DIM = 5120  #  pooling dims (2*C*T)



python convert_audio_input.py <wav_data_speaker.h> <audio_input.bin>
example:

cd /mnt/d/software/IP7AI/ipro_sdk/apps/platform/ipro7_demo/demo/ai/tools
python convert_audio_input.py wav_data_speaker1_0.h audio_input.bin

使用流程

  ┌─────────────────────────────┐
  │                                                          │
  │  wav_data_speaker1_0.h                                   │
  │         │                                               │
  │         ▼ (convert_audio_input.py)                      │
  │                                                          │
  │  audio_input.bin  ──────► SD 卡                    │
  │  (raw int16 PCM)         /sdcard/ai_model/audio_input.bin│
  │                                                          │
  │         │                                               │
  │         ▼ (ai_run_demo 4 或 ai_run_ext 4)               │
  │                                                          │
  │  ┌──────────────────┐               │
  │  │ speaker_recog_preprocess_pcm()     │               │
  │  │ (MFCC 前處理)                      │               │
  │  │ - int16 → float 正規化            │               │
  │  │ - DC offset removal                │               │
  │  │ - Pre-emphasis                     │               │
  │  │ - Modified Hann window             │               │
  │  │ - FFT → Power spectrum            │               │
  │  │ - Mel filterbank (80 bins)         │               │
  │  │ - Log                              │               │
  │  │ - subtract_mean                    │               │
  │  └──────────────────┘               │
  │         │                                              │
  │         ▼                                              │
  │  MFCC Features [298][80]                                │
  │         │                                              │
  │         ▼ (NPU 推理)                                   │
  │                                                         │
  │  Speaker Embedding [48]                                 │
  │                                                         │
  └─────────────────────────────┘
