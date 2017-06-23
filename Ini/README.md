例:
```
void test() {
	Property::create("base", "./base.prop");
	Property::create("win", "./win.prop");
	
	Property::m("base").set("eee", "EEE");
	Property::m("base").remove("ccc");
	printf("%s\n", Property::m("base").get("bbb").c_str());
	
	Property::m("win").set("abc", "ABDFAFDFA");
	
	Property::purge();
}
```
