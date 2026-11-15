package com.rujiangiot.loraenvassistant_androidapp.Tools.Dialog;

import android.app.AlertDialog;
import android.content.Context;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.rujiangiot.loraenvassistant_androidapp.Tools.Dialog.Interface.DialogListener;

/** 可以将这两个对话框方法重构为一个独立工具类：
 * 核心思路是将界面展示与业务处理解耦：工具类只负责构建和显示 {@link AlertDialog}，通过回调接口把用户输入结果传递给调用方，
 * 具体的数据操作（例如 {@link itemList#add()}、{@link notifyDataSetChanged()}）由调用者在回调中自行完成。
 * 静态类无需构造函数。
 */
public class DialogListenerImpl implements DialogListener {
    /**
     * 显示带单个输入框的对话框
     *
     * @param context  上下文（建议使用 Activity）
     * @param title    对话框标题
     * @param hint     输入框提示文字（可为 null）
     * @param listener 确定按钮回调（输入非空时触发）
     */
    public static void showSingleInputDialog(@NonNull Context context,
                                             @NonNull String title,
                                             @Nullable String hint,
                                             @NonNull OnSingleInputListener listener) {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setTitle(title);

        final EditText input = new EditText(context);
        if (hint != null) {
            input.setHint(hint);
        }
        builder.setView(input);

        builder.setPositiveButton("确定", (dialog, which) -> {
            String text = input.getText().toString().trim();
            if (!text.isEmpty()) {

                listener.onInputConfirmed(text);                                                                // 通过接口传递数据
            } else {
                Toast.makeText(context, "内容不能为空", Toast.LENGTH_SHORT).show();
            }
        });
        builder.setNegativeButton("取消", null);
        builder.show();
    }

    /**
     * 显示带多个输入框的对话框
     *
     * @param context  上下文
     * @param title    对话框标题
     * @param editTextHints    各输入框的提示文字数组（数量决定输入框个数）
     * @param listener 确定按钮回调（所有输入非空时触发）
     */
    public static void showMultiInputsDialog(@NonNull Context context,
                                             @NonNull String title,
                                             @NonNull String[] editTextHints,
                                             @NonNull DialogListener.OnMultiInputListener listener) {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setTitle(title);

        /* 动态创建一个新的layout界面 */
        LinearLayout layout = new LinearLayout(context);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(50, 20, 50, 0);

        /* 填充布局，循环填充数组 */
        final EditText[] editTextInputs = new EditText[editTextHints.length];
        for (int i = 0; i < editTextHints.length; i++) {
            EditText editText = new EditText(context);
            editText.setHint(editTextHints[i]);
            layout.addView(editText);
            editTextInputs[i] = editText;
        }
        builder.setView(layout);

        builder.setPositiveButton("确定", (dialog, which) -> {
            String[] results = new String[editTextInputs.length];
            boolean allFilled = true;
            for (int i = 0; i < editTextInputs.length; i++) {
                results[i] = editTextInputs[i].getText().toString().trim();
                if (results[i].isEmpty()) {
                    allFilled = false;
                    break;
                }
            }
            if (allFilled) {
                listener.onInputsConfirmed(results);                                                        // 通过接口传递数据
            } else {
                Toast.makeText(context, "内容不能为空", Toast.LENGTH_SHORT).show();
            }
        });
        builder.setNegativeButton("取消", null);
        builder.show();
    }

//    public static void showAlertDialog(Context context, String title, String message, String postiveButtonText, String negativeButtonText) {
//        new AlertDialog.Builder(context)
//                .setTitle(title)
//                .setMessage(message)
//                .setPositiveButton(postiveButtonText, )
//    }
}