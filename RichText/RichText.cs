using System;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using UnityEngine;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using System.Text;


/**
 * image: <image=name size=20>  image:necessary others:optional
 * link: <link=funid arg=123456 color=blue text=[冰花]>  link,text:necessary others:optional
 * anim: <ani=frame1,frame2,frame3 fps=5 size=20>  ani:necessary  others:optional
 */

[AddComponentMenu("UI/RichText")]
public class RichText : Text, IPointerClickHandler
{
	//regex defines
	private static Regex cmdRegex_		= new Regex (@"(<[^>]+>)");
	private static Regex cmdNameRegex_	= new Regex (@"<([^= ]+)");

	private const string imageSign_		= "image";
	private static Regex imgNameRegex_	= new Regex (@"image=([\w_]+)");
	private static Regex sizeRegex_		= new Regex (@"size=(\d+)");

	private const string linkSign_		= "link";
	private static Regex funcIdRegex_	= new Regex (@"link=(\d+)");
	private static Regex argRegex_ 		= new Regex (@"arg=([^ >]+)");
	private static Regex colorRegex_	= new Regex (@"color=([^ >]+)");
	private static Regex textRegex_ 	= new Regex (@"text=([^ >]+)");

	private const string animSign_ 		= "anim";
	private static Regex animRegex_ 	= new Regex (@"anim=([^ >]+)");
	private static Regex frameRegx_		= new Regex (@"([^,]+)");
	private static Regex fpsRegex_		= new Regex (@"fps=(\d+)");

	//translate
	private string translateText_;
	private static StringBuilder strBuilder_ = new StringBuilder();

	//image
	private List<int> imageVertexs_ = new List<int> ();
	private List<Image> imagePool_	= new List<Image> ();
	private static DefaultControls.Resources resources_ = new DefaultControls.Resources ();

	//link
	private class LinkInfo {
		public int funcId;
		public string arg;
		public Color color;
		public int startIdx;
		public int endIdx;
		public List<Rect> boxes = new List<Rect>();
	}
	private List<LinkInfo> linkInfos_ = new List<LinkInfo> ();

	public bool hideLinkUnderline;

	//animation
	private class AnimInfo {
		public Image image;
		public List<Sprite> frames = new List<Sprite>();
		public int curFrameIdx;
		public float frameInterval;
		public float startTime;
	}
	private List<AnimInfo> animInfos_ = new List<AnimInfo>();



	public override void SetVerticesDirty()
	{
		base.SetVerticesDirty();
		UpdateContent ();
	}

	public void UpdateContent()
	{
		#if UNITY_EDITOR
		if (UnityEditor.PrefabUtility.GetPrefabType(this) == UnityEditor.PrefabType.Prefab)
			return;
		#endif

		Translate ();
	}

	protected void Translate()
	{
		var index = 0;
		linkInfos_.Clear ();
		imageVertexs_.Clear ();
		imagePool_.Clear ();
		animInfos_.Clear ();
		strBuilder_.Length = 0;

		GetComponentsInChildren<Image> (true, imagePool_);

		foreach (Match match in cmdRegex_.Matches(m_Text)) {

			var cmdName = cmdNameRegex_.Match(match.Value).Groups[1].Value; //Matches can use .Value, Match needs use .Groups[n].Value

			strBuilder_.Append (m_Text.Substring(index, match.Index-index));

			switch (cmdName) {
			case imageSign_:
				TranslateImage (match.Value, ref index);
				break;
			case linkSign_:
				TranslateLink (match.Value, ref index);
				break;
			case animSign_:
				TranslateAnim (match.Value, ref index);
				break;
			default:
				strBuilder_.Append (match.Value);
				break;
			}

			index = match.Index + match.Length;
		}

		strBuilder_.Append (m_Text.Substring(index, text.Length-index));

		translateText_ = strBuilder_.ToString ();

		for (var i = imageVertexs_.Count; i < imagePool_.Count; i++) {
			imagePool_ [i].transform.gameObject.SetActive (false);
		}
	}

	#region image
	protected void TranslateImage(string str, ref int index)
	{
		if (!imgNameRegex_.IsMatch (str)) {
			strBuilder_.Append (str);
			return;
		}
		var name = imgNameRegex_.Match(str).Groups[1].Value;

		var size = fontSize;
		if (sizeRegex_.IsMatch(str)) {
			size = int.Parse(sizeRegex_.Match(str).Groups[1].Value);
		}

		imageVertexs_.Add(strBuilder_.Length * 4);

		strBuilder_.Append("<quad width=1 size=" + size + "/>");

		Image img = null;
		if (imageVertexs_.Count <= imagePool_.Count) {
			img = imagePool_ [imageVertexs_.Count - 1];
			img.transform.gameObject.SetActive (true);

		} else {
			var go = DefaultControls.CreateImage (resources_);
			go.layer = gameObject.layer;
			var rt = go.gameObject.transform;
			rt.SetParent (rectTransform);
			rt.localPosition = Vector3.zero;
			rt.localRotation = Quaternion.identity;
			rt.localScale = Vector3.one;
			img = go.GetComponent<Image> ();
			imagePool_.Add (img);
		}

		if (!img.sprite || img.sprite.name != name) {
			img.sprite = Resources.Load<Sprite> (name);
		}

		img.rectTransform.sizeDelta = new Vector2 (size, size);
	}
	#endregion

	#region link
	protected void TranslateLink(string str, ref int index)
	{
		if (!funcIdRegex_.IsMatch(str) || !textRegex_.IsMatch(str)) {
			strBuilder_.Append (str);
			return;
		}

		var funcId = int.Parse(funcIdRegex_.Match(str).Groups[1].Value);
		var text = textRegex_.Match(str).Groups[1].Value;

		var arg = "";
		if (argRegex_.IsMatch(str)) {
			arg = argRegex_.Match(str).Groups[1].Value;
		}

		var colorStr = "blue";
		if (colorRegex_.IsMatch(str)) {
			colorStr = colorRegex_.Match(str).Groups[1].Value;
		}

		Color color;
		if (!ColorUtility.TryParseHtmlString (colorStr, out color)) {
			color = Color.blue;
		}

		strBuilder_.Append("<color=" + colorStr + ">");

		linkInfos_.Add (new LinkInfo {
			funcId = funcId,
			arg = arg,
			color = color,
			startIdx = strBuilder_.Length * 4,
			endIdx = (strBuilder_.Length + text.Length) * 4 + 3
		});

		strBuilder_.Append(text);
		strBuilder_.Append("</color>");
	}

	void AddUnderline(VertexHelper toFill, IList<UIVertex> verts, Color color, float startx, float endx, float y)
	{
		float unitWidth = Math.Abs (verts[0].position.x - verts[1].position.x);
		float unitHeight = fontSize * 0.2f;

		Vector3[] positions = new Vector3[4];
		positions [0] = new Vector3 (startx, y, 0.0f);
		positions [1] = positions [0] + new Vector3 (unitWidth, 0.0f, 0.0f);
		positions [2] = positions [1] + new Vector3 (0.0f, -unitHeight, 0.0f);
		positions [3] = positions [0] + new Vector3 (0.0f, -unitHeight, 0.0f);

		UIVertex[] tmpVerts = new UIVertex[4];

		while (positions [0].x < endx-unitWidth*0.5) { //unitWidth*0.5 : used for adjust
			positions [1].x = positions [0].x + unitWidth;
			positions [2].x = positions [1].x;
			positions [3].x = positions [0].x;

			for (int i = 0; i < 4; i++) {
				tmpVerts [i] = verts [i];
				tmpVerts [i].color = color;
				tmpVerts [i].position = positions [i];
			}
			toFill.AddUIVertexQuad (tmpVerts);

			positions [0].x += unitWidth-3.5f; //3.5f : used for adjust
		}
	}
	#endregion

	#region animation
	protected void TranslateAnim(string str, ref int index)
	{
		if (!animRegex_.IsMatch (str)) {
			strBuilder_.Append (str);
			return;
		}

		AnimInfo info = new AnimInfo ();
		if (fpsRegex_.IsMatch (str)) {
			info.frameInterval = 1.0f / float.Parse (fpsRegex_.Match (str).Groups[1].Value);
		} else {
			info.frameInterval = 0.2f; //default
		}

		var size = fontSize;
		if (sizeRegex_.IsMatch (str)) {
			size = int.Parse (sizeRegex_.Match (str).Groups[1].Value);
		}

		foreach (Match match in frameRegx_.Matches(animRegex_.Match(str).Groups[1].Value)) {
			info.frames.Add (Resources.Load<Sprite> (match.Value));
		}

		if (info.frames.Count > 0) {

			imageVertexs_.Add (strBuilder_.Length * 4);
			strBuilder_.Append ("<quad width=1 size=" + size + "/>");

			Image img = null;
			if (imageVertexs_.Count <= imagePool_.Count) {
				img = imagePool_ [imageVertexs_.Count - 1];
				img.transform.gameObject.SetActive (true);

			} else {
				var go = DefaultControls.CreateImage (resources_);
				go.layer = gameObject.layer;
				var rt = go.gameObject.transform;
				rt.SetParent (rectTransform);
				rt.localPosition = Vector3.zero;
				rt.localRotation = Quaternion.identity;
				rt.localScale = Vector3.one;
				img = go.GetComponent<Image> ();
				imagePool_.Add (img);
			}

			info.image = img;
			info.startTime = Time.time;
			info.curFrameIdx = 0;

			img.sprite = info.frames [0];
			img.rectTransform.sizeDelta = new Vector2 (size, size);

			animInfos_.Add (info);

		} else {
			strBuilder_.Append (str);
		}
	}
	#endregion

	protected override void OnPopulateMesh (VertexHelper toFill)
	{
		string tmp = m_Text;
		m_Text = translateText_;
		base.OnPopulateMesh(toFill);
		m_Text = tmp;

		UIVertex vertex = new UIVertex();

		for (var i = 0; i < imageVertexs_.Count; i++) {

			//erase black dots
			var idx = imageVertexs_ [i];
			if (idx >= toFill.currentVertCount)
				continue;
			toFill.PopulateUIVertex (ref vertex, idx);
			for (var j = idx + 1; j < idx + 4 && j < toFill.currentVertCount; j++) {
				toFill.SetUIVertex (vertex, j);
			}

			//set image position
			var rt = imagePool_ [i].rectTransform;
			var size = rt.sizeDelta;
			rt.anchoredPosition = new Vector2 (vertex.position.x+size.x*0.5f, vertex.position.y-size.y*0.5f);
		}

		//link item click rect
		foreach (var info in linkInfos_) {
			
			info.boxes.Clear();

			if (info.startIdx >= toFill.currentVertCount)
				continue;

			toFill.PopulateUIVertex(ref vertex, info.startIdx);
			var pos = vertex.position;
			var bounds = new Bounds(pos, Vector3.zero);
			for (int i = info.startIdx; i < info.endIdx && i < toFill.currentVertCount; i++) {

				toFill.PopulateUIVertex(ref vertex, i);
				pos = vertex.position;
				if (pos.x < bounds.min.x) { // changeline 
					info.boxes.Add(new Rect(bounds.min, bounds.size));
					bounds = new Bounds(pos, Vector3.zero);
				} else {
					bounds.Encapsulate(pos);
				}
			}
			info.boxes.Add(new Rect(bounds.min, bounds.size));
		}

		//add underline
		if (!hideLinkUnderline) {
			Vector2 extents = rectTransform.rect.size;
			var settings = GetGenerationSettings (extents);
			TextGenerator underlineGen = new TextGenerator ();
			underlineGen.Populate ("_", settings);
			IList<UIVertex> underlineVerts = underlineGen.verts;

			foreach (var info in linkInfos_) {
				if (info.startIdx >= toFill.currentVertCount) {
					continue;
				}

				for (int i = 0; i < info.boxes.Count; i++) {
					AddUnderline (
						toFill,
						underlineVerts,
						info.color,
						info.boxes [i].x,
						info.boxes [i].x + info.boxes [i].width,
						info.boxes [i].y);
				}
			}
		}
	}

	public void OnPointerClick (PointerEventData eventData)
	{
		Vector2 localPosition;
		RectTransformUtility.ScreenPointToLocalPointInRectangle (
			rectTransform,
			eventData.position,
			eventData.pressEventCamera,
			out localPosition
		);

		foreach (var info in linkInfos_) {
			foreach (var rect in info.boxes) {
				if (rect.Contains (localPosition)) {
					Debug.Log ("OnPointerClick " + info.arg);
					return;
				}
			}
		}
	}

	public void Update()
	{
		foreach (AnimInfo info in animInfos_) {
			if (info.startTime + (info.curFrameIdx+1) * info.frameInterval < Time.time) {
				info.curFrameIdx++;
				if (info.curFrameIdx >= info.frames.Count) {
					info.curFrameIdx = 0;
					info.startTime = Time.time;
				}
				info.image.sprite = info.frames [info.curFrameIdx];
			}
		}
	}
}


