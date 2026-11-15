package com.rujiangiot.loraenvassistant_androidapp.Tools.Dialog.Interface;

import androidx.annotation.NonNull;

public interface DialogListener {
    /* 该接口用于将输入框的结果回调给调用方 */
    /* 单输入框回调 */
    public interface OnSingleInputListener {
        void onInputConfirmed(@NonNull String input);
    }

    /* 多输入框回调，输入值按顺序以数组传递 */
    public interface OnMultiInputListener {
        void onInputsConfirmed(@NonNull String[] inputs);
    }
}
