// © Copyright 2023, D0MlNIC, All Rights Reserved.

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int z = 0;
// Struct Definitions
struct XML_Document {
    char *filepath; // A
    char *data; // A
    int len;
};

struct XML_Attributes {
    char **aKey; // A
    char **aVal; // A
};

/*struct XML_ChildData {
    int *child_isSimple;     // A
    char **child_name;       // A
    char **child_sPtr;       // A
    char **child_ePtr;       // A
};*/

struct XML_Node {
    bool has_attributes; //y
    bool has_spaces; // y
    bool has_crlf;   // y
    bool is_simple;  
    
    //int num_attributes;
    int success;
    int node_name_len; //y
    int depth; //y
    int num_childs; 
    int num_sep; //y
    int num_attr;//y

    //int *lt_i; // Indexes of starting point // A
    //int *gt_i; // Indexes of ending point   // A
    
    char *node_name;//y                        // A
    char *node_sPtr;   //y
    char *node_ePtr;   //y
    
    //struct XML_ChildData *cd;
    struct XML_Attributes *attr; //y
    struct XML_Document *doc; //y
    struct XML_Node **child_node; // Pointer to Pointers to Structs
};


// Function Header Prototype

/**
 * Type of Functions:
 * 1) xml_load_
 * 2) xml_parse_
 * 3) xml_extract_
 * 4) xml_has_
 * 5) xml_get_
 * 6) xml_set_
 * 7) xml_is_
 * 8) xml_free_
 */

//// Public Functions
struct XML_Document *xml_load_document(char *filepath);
struct XML_Node *xml_parse(struct XML_Document *doc);
void xml_free_document(struct XML_Document *doc);
void xml_free_node(struct XML_Node *node, bool free_doc);

//// Private Function
struct XML_Node *xml_parse_childs(struct XML_Document *doc, int uDepth, char *sPtr, char name[]);
int xml_parse_spaces(struct XML_Node *node, char *sPtr);
int xml_extract_attributes(struct XML_Node *node);
int xml_extract_name(struct XML_Node *node, char *sPtr);
bool xml_has_attributes(char *sPtr);
void xml_free_attributes(struct XML_Attributes *attr, int numA);

// Function Declaration
struct XML_Document *xml_load_document(char *filepath) {
    FILE *xml_file;
    struct XML_Document *doc;

    doc = malloc(sizeof(struct XML_Document));
    doc->filepath = malloc(strlen(filepath)+1);
    z += strlen(filepath)+1;
    strcpy(doc->filepath, filepath);
    xml_file = fopen(filepath, "rb");
    
    fseek(xml_file, 0, SEEK_END);
    doc->len = ftell(xml_file);
    doc->data = calloc(doc->len+1, 1);   // Account for null byte ('\0')
    z += doc->len+1;
    rewind(xml_file);
    fread(doc->data, 1, doc->len, xml_file);
    
    fclose(xml_file);
    return doc;
}

struct XML_Node *xml_parse(struct XML_Document *doc) {
    
    // Defining Structs
    struct XML_Node *root, *child;
    struct XML_Attributes *attr;

    // Defining Variables
    int code;
    int name_len, depth, temp, n, t;
    int temp_child_name_len, temp_num_childs;
    bool root_node_found;

    // Declaring Structs
    root = malloc(sizeof(struct XML_Node));
    z += sizeof(struct XML_Node);

    // Declaring Variables
    
    root->node_sPtr = NULL;
    root->node_ePtr = NULL;
    root->node_name = NULL;
    root->num_childs = 0;
    root->num_attr = 0;
    root->node_name_len = 0;
    root->is_simple = false;
    root->child_node = NULL;
    root->attr = NULL;
    root->doc = doc;
    depth = 0;

    for (n = 0; n < doc->len; n++) {
        if (doc->data[n] == '<') {

            // Finding declarations and making sure that it is not between the root node->
            if ((doc->data[n+1] == '!') || (doc->data[n+1] == '#') || (doc->data[n+1] == '?')) {
                if (depth == 0) {
                    //declarations_found = 1;
                    // Finding closing tags of the declaration to skip iterations over the main loop as it's not going to be optimal->
                    for (t=n+1; t<doc->len; t++) {
                        if (doc->data[t] == '>') {
                            temp = t+1;
                            break;
                        }
                    }
                    n = temp;
                    continue;
                } else {
                    root->success == 1001; // Declaration in between root nodes
                    break;
                }
            } else {
                if (root->node_sPtr == NULL) {
                    // Setting root
                    root->node_sPtr = (doc->data + n);
                    code = xml_extract_name(root, root->node_sPtr);
                    //code = xml_extract_attributes(root);
                    //n += code;
                    
                    // Temporary
                    while(doc->data[n] != '>') {
                        n++;
                    }

                    //xml_free_attributes(root->attr, root->num_attr);

                    code = xml_parse_spaces(root, (doc->data+n));
                    n += code;
                }

                child = xml_parse_childs(doc, depth, (doc->data + n), root->node_name);

                if (child == NULL) {
                    while (doc->data[n] != '<') {
                        n++;
                    }
                    if (doc->data[n+1] == '/') {
                        root->node_ePtr = (doc->data + n);
                        root->depth = depth;
                        break;
                    }
                    //n = root->child_node[ root->num_childs - 1 ]->node_ePtr - doc->data;
                    //continue;
                    root->success = 1001; // Root node is empty
                } else if (child != NULL) {
                    root->child_node = realloc(root->child_node, sizeof(struct XML_Node *) * (root->num_childs + 1));
                    z += sizeof(struct XML_Node *);
                    //root->child_node[root->num_childs] = malloc(sizeof(struct XML_Node));
                    root->child_node[root->num_childs] = child; // This is the pointer stored in index x of child_node.
                    root->num_childs++;
                    n = (root->child_node[root->num_childs - 1]->node_ePtr - doc->data + root->child_node[root->num_childs - 1]->node_name_len + 2); // n +
                    //n--;
                }
            }
        }
    }
    return root;
}

struct XML_Node *xml_parse_childs(struct XML_Document *doc, int uDepth, char *sPtr, char name[]) { // Upper Depth
    
    // Defining Structs
    struct XML_Node *node, *child;
    struct XML_Attributes *attr;

    // Defining Variables
    int code, eName, eAttr, eSpac;
    int name_len, depth, temp, n, t;
    int temp_child_name_len, temp_num_childs;
    //bool node_found;

    // Declaring Structs
    node = malloc(sizeof(struct XML_Node));
    z += sizeof(struct XML_Node);

    // Declaring Variables
    
    node->node_sPtr = NULL;
    node->node_ePtr = NULL;
    node->node_name = NULL;
    node->num_childs = 0;
    node->node_name_len = 0;
    node->num_attr = 0;
    node->is_simple = false;
    node->child_node = NULL;
    node->attr = NULL;
    node->doc = doc;
    code = 0;
    depth = uDepth + 1;

    for (n = 0; n < doc->len; n++) {
        if (sPtr[n] == '<') {
            if (sPtr[n+1] == '/') {
                if (!(memcmp((sPtr+n+2), name, 4) == 0)) {
                    node->node_ePtr = (sPtr + n);
                    node->depth = depth;
                    if (node->num_childs == 0) {
                        node->is_simple = true;
                    } else {
                        node->is_simple = false;
                    }
                    break;
                } else {
                    free(node);
                    return NULL;
                }
            } else {
                if (node->node_sPtr == NULL) {
                    node->node_sPtr = sPtr;
                    eName = xml_extract_name(node, node->node_sPtr);
                    //eAttr = xml_extract_attributes(node);
                    //n += eAttr;

                    // Temporary
                    while (sPtr[n] != '>') {
                        n++;
                    }

                    eSpac = xml_parse_spaces(node, (sPtr+n));
                    n += eSpac;
                }

                child = xml_parse_childs(doc, depth, (sPtr + n), node->node_name);
                if (child != NULL) {
                    node->child_node = realloc(node->child_node, sizeof(struct XML_Node *) * (node->num_childs + 1));
                    z += sizeof(struct XML_Node *);
                    //node->child_node[node->num_childs] = malloc(sizeof(struct XML_Node));
                    node->child_node[node->num_childs] = child; // This is the pointer stored in index x of child_node.
                    node->num_childs++;
                    n = (node->child_node[node->num_childs - 1]->node_ePtr - sPtr + node->child_node[node->num_childs - 1]->node_name_len + 2); // n +
                    //n--; // Because of for loop
                } else if (child == NULL) {
                    while (sPtr[n] != '<') {
                        n++;
                    }
                    if (sPtr[n+1] == '/') {
                        node->node_ePtr = (sPtr + n);
                        node->depth = depth;
                        if (node->num_childs == 0) {
                            node->is_simple = true;
                        } else {
                            node->is_simple = false;
                        }
                        break;
                    }
                }
            }
        }
    }
    return node;
}

int xml_parse_spaces(struct XML_Node *node, char *sPtr) {
    int n;
    
    node->has_crlf = true;
    node->has_spaces = true; // Assuming spaces and \r\n exists
    node->num_sep = 0;
    n = 0;

    if (sPtr[n] == '>') {
        n++;
    }

    // Getting ahead of crlf characters
    if (sPtr[n] == '\r' && sPtr[n+1] == '\n') {
        n += 2;
    } else if (sPtr[n] == '\n') {
        n++;
        node->has_crlf = false;
    }

    if (sPtr[n] == '\t') {
        node->has_spaces = false;
    }
    
    while (sPtr[n] != '<') {
        node->num_sep++;
        n++;
    }
    node->num_sep--; // Gets incremented upto '<' sign
    return n;
}

int xml_extract_name(struct XML_Node *node, char *sPtr) {
    int len;
    char *name;

    len = 0;

    if (sPtr[0] == '<') {
        sPtr++;
    }

    while ((sPtr[len] != ' ') && (sPtr[len] != '>')) {
        len++;
    }

    name = calloc(len+1, 1);
    z += len+1;
    memcpy(name, sPtr, len);
    
    node->node_name = name;
    node->node_name_len = len;
    return 1;
}


/// @brief Extracts The Attributes of any node
/// @param node The Node Struct Containing all info
/// @return Returns -1 on error otherwise the index of '>' (closing sign of a node)
int xml_extract_attributes(struct XML_Node *node) {

    int n, num_attr, quotation_count;
    int attr_key_len, attr_val_len;
    char *sPtr;
    struct XML_Attributes *attr;
    
    if (!xml_has_attributes(node->node_sPtr + node->node_name_len + 1)) { // +1 to go the next index from where the name is ending.
        node->has_attributes = false;
        node->success = 1002;
        return node->node_name_len + 1; // Node does not have any attributes.
    } else {
        node->has_attributes = true;
    }

    n = node->node_name_len + 1;
    attr_key_len = 0;
    attr_val_len = 0;
    quotation_count = 0;
    num_attr = 0;
    sPtr = node->node_sPtr;
    attr = malloc(sizeof(struct XML_Attributes));
    attr->aKey = NULL;
    attr->aVal = NULL;

    while (sPtr[n] != '>') {
        // Going over spaces because you never know how many spaces there are instead of the default 1 between pairs.
        while (sPtr[n] == ' ') {
            n++;
        }
        
        // Getting the key length here
        while ((sPtr[n] != '=') && (sPtr[n] != ' ')) {
            attr_key_len++;
            n++;
        }

        // Adding the key to the node
        attr->aKey = realloc(attr->aKey, (num_attr + 1) * sizeof(char *));
        attr->aKey[num_attr] = calloc(attr_key_len + 1, 1);
        memcpy(attr->aKey[num_attr], (sPtr + n - attr_key_len), attr_key_len);

        // Incrementing n until '"' found
        while (sPtr[n] != '"') {
            n++;
        }

        // Getting the length of value here
        while(quotation_count != 2) {
            if (sPtr[n] == '"') {
                quotation_count++;
            } else {
                attr_val_len++;
            }
            n++;
        }

        // Adding the value to the node
        attr->aVal = realloc(attr->aVal, (num_attr + 1) * sizeof(char *));
        attr->aVal[num_attr] = calloc(attr_val_len + 1, 1);
        memcpy(attr->aVal[num_attr], (sPtr + n - attr_val_len - 1), attr_val_len);

        num_attr++;

        while (sPtr[n] == ' ') {
            n++;
        }

        // Setting the control Parameters back to 0
        attr_key_len = 0;
        attr_val_len = 0;
        quotation_count = 0;
    }

    node->num_attr = num_attr;
    node->attr = attr;
    return n;
}

bool xml_has_attributes(char *sPtr) {
    if (sPtr[0] == ' ') {
        return true;
    } else if (sPtr[0] == '>') {
        return false;
    }
}

void xml_free_document(struct XML_Document *doc) {
    free(doc->data);
    free(doc->filepath);
    free(doc);
}

void xml_free_attributes(struct XML_Attributes *attr, int numA) {
    for (int n = 0; n < numA; n++) {
        free(attr->aKey[n]);
        free(attr->aVal[n]);
    }
    free(attr->aKey);
    free(attr->aVal);
    free(attr);
}

void xml_free_node(struct XML_Node *node, bool free_doc) {
    free(node->node_name);
    
    for (int n = 0; n < node->num_childs; n++) {
        xml_free_node(node->child_node[n], false);
    }

    if (node->num_attr) {
        xml_free_attributes(node->attr, node->num_attr);
    }
    free(node->child_node);

    if (free_doc) {
        xml_free_document(node->doc);
    }

    free(node);
}

