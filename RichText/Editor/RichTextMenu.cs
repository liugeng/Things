using System;
using System.Reflection;
using UnityEditor;
using UnityEngine;
using UnityEngine.UI;

public static class RichTextMenu
{
	private static MethodInfo placeUIElementRoot_;

	[MenuItem("GameObject/UI/RichText", false)]
	public static void AddRichText(MenuCommand cmd)
	{
		GameObject child = new GameObject("RichText");
		RectTransform rectTransform = child.AddComponent<RectTransform>();
		rectTransform.sizeDelta = new Vector2(160f, 30f);
		RichText text = child.AddComponent<RichText>();
		text.text = "RichText";
		text.fontSize = 20;
		text.color = new Color(0f, 0f, 0f, 1f);
		#if UNITY_5_3_OR_NEWER
		text.alignByGeometry = true;
		#endif

		PlaceUIElementRoot(child, cmd);
	}

	private static void PlaceUIElementRoot(GameObject element, MenuCommand cmd)
	{
		if (placeUIElementRoot_ == null)
		{
			Assembly assembly = Assembly.GetAssembly(typeof(UnityEditor.UI.TextEditor));
			Type type = assembly.GetType("UnityEditor.UI.MenuOptions");
			if (type == null)
				Debug.Log ("type is null");
			placeUIElementRoot_ = type.GetMethod("PlaceUIElementRoot", BindingFlags.NonPublic | BindingFlags.Static);
		}
		placeUIElementRoot_.Invoke(null, new object[] {element, cmd});
	}
}


