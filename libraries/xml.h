#ifndef XML_HM
#define XML_HM

struct XML_Document;
struct XML_Attributes;
struct XML_RootNode;
struct XML_ChildNode;

struct XML_Document *xml_load_document(char *filepath);
struct XML_RootNode *xml_get_root_node(struct XML_Document *doc, const char root_name[]);
struct XML_ChildNode *xml_get_child_node(int child_num, struct XML_Document *doc, struct XML_RootNode *root, struct XML_ChildNode *node);
char *xml_get_value(struct XML_Document *doc, struct XML_ChildNode *node, char aKey[], char aVal[], char cName[]);
void xml_free_doc(struct XML_Document *doc);
void xml_free_attr(struct XML_Attributes *attr);
void xml_free_root(struct XML_RootNode *root);
void xml_free_child(struct XML_ChildNode *child);

#endif