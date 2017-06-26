例:
```
void test() {
	Ini::create("base", "./base.ini");
	Ini::create("win", "./win.ini");
	
	Ini::m("base").set("eee", "EEE");
	Ini::m("base").remove("ccc");
	printf("%s\n", Ini::m("base").get("bbb").c_str());
	
	Ini::m("win").set("mr_file37.mod", "ABDFAFDFA");
	
	Ini::purge();
}
```
