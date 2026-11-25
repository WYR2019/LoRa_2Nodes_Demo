package com.rujiangiot.loraenvassistant_androidapp.Tools.JSON;

import androidx.annotation.NonNull;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * 深度遍历 JSON，提取任意层级的目标键并返回 Map
 */
public class JSONHelper {

    @NonNull
    public static List<String> parseKeys(String jsonString, String... targetKeys) {
        List<String> foundKeys = new ArrayList<>();
        if (jsonString == null || jsonString.trim().isEmpty()) return foundKeys;

        try {
            JSONObject root = new JSONObject(jsonString);
            Set<String> targetSet = new HashSet<>(Arrays.asList(targetKeys));
            traverseForKeys(root, targetSet, foundKeys);
        } catch (JSONException e) {
            // 返回空列表
        }
        return foundKeys;
    }

    // 递归辅助方法
    private static void traverseForKeys(Object node, Set<String> targetSet, List<String> foundKeys) throws JSONException {
        if (node instanceof JSONObject) {
            JSONObject obj = (JSONObject) node;
            Iterator<String> keys = obj.keys();
            while (keys.hasNext()) {
                String key = keys.next();
                if (targetSet.contains(key)) {
                    foundKeys.add(key);                                                             // 只收集键名
                }
                traverseForKeys(obj.get(key), targetSet, foundKeys);
            }
        } else if (node instanceof JSONArray) {
            JSONArray array = (JSONArray) node;
            for (int i = 0; i < array.length(); i++) {
                traverseForKeys(array.get(i), targetSet, foundKeys);
            }
        }
    }

    /**
     * 从 JSON 字符串中提取指定键的第一个值
     * @param jsonString 原始 JSON 字符串
     * @param keys       需要查找的键名
     * @return 键值对 Map（保持传入顺序），未找到的键对应值为 null
     */
    public static Map<String, String> parseValues(String jsonString, String... keys) {
        Map<String, String> result = new LinkedHashMap<>();
        if (jsonString == null || jsonString.trim().isEmpty()) {
            /* 初始化结果，所有目标键均为 null */
            for (String key : keys) {
                result.put(key, null);
            }
            return result;
        }

        try {
            JSONObject root = new JSONObject(jsonString);
            /* 用于记录哪些键已经找到 */
            Map<String, Boolean> foundFlags = new HashMap<>();
            for (String key : keys) {
                foundFlags.put(key, false);
            }
            traverseForValues(root, keys, result, foundFlags);
        } catch (JSONException e) {
            /* 解析失败时，返回全 null 的结果 */
            for (String key : keys) {
                result.putIfAbsent(key, null);
            }
        }
        return result;
    }

    /**
     * 递归遍历 JSON 节点
     */
    private static void traverseForValues(Object node, String[] targetKeys,
                                 Map<String, String> result,
                                 Map<String, Boolean> foundFlags) throws JSONException {
        if (node instanceof JSONObject) {
            JSONObject obj = (JSONObject) node;
            Iterator<String> iterator = obj.keys();
            while (iterator.hasNext()) {
                String key = iterator.next();
                Object value = obj.get(key);

                /* 检查是否是目标键且尚未找到 */
                for (String target : targetKeys) {
                    if (target.equals(key) && Boolean.FALSE.equals(foundFlags.get(target))) {
                        result.put(target, value.toString());
                        foundFlags.put(target, true);
                        // 如果所有键都已找到，可以提前结束（但此处继续，不影响）
                    }
                }

                /* 递归处理子节点 */
                traverseForValues(value, targetKeys, result, foundFlags);
            }
        } else if (node instanceof JSONArray) {
            JSONArray array = (JSONArray) node;
            for (int i = 0; i < array.length(); i++) {
                traverseForValues(array.get(i), targetKeys, result, foundFlags);
            }
        }
        // 基本类型不处理
    }
}