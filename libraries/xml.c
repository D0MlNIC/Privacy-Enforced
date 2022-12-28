#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * I know there is a lot of duplicate code here.
 * Just for the time being I wanted it working.
 * Over time I will actually refactor it, and maybe change a lot of this file, coz I already have some ideas.
 * So, just to say, it's working for now. Expect changes in the future.
 */

// Struct Definitions
struct XML_Document {
    char *filepath; // A
    char *data; // A
    int len;
};

struct XML_Attributes {
    int numA;
    char **aKey; // A
    char **aVal; // A
};

struct XML_RootNode {
    bool has_attributes;
    bool has_spaces;
    bool has_crlf;
    int success;
    int num_childs;
    int root_name_len;
    int num_attributes;
    int depth;
    int num_sep;
    int *child_isSimple;     // A
    char *root_name;         // A
    char *root_sPtr;   
    char *root_ePtr;   
    char **attributes_key;   // A
    char **attributes_value; // A
    char **child_name;       // A
    char **child_sPtr;       // A
    char **child_ePtr;       // A
};

struct XML_ChildNode {
    bool has_attributes;
    bool has_spaces;
    bool has_crlf;
    int success;
    int num_childs;
    int node_name_len;
    int num_attributes;
    int depth;
    int num_sep;
    int *child_isSimple;     // A
    char *node_name;         // A
    char *node_sPtr;   
    char *node_ePtr;   
    char **attributes_key;   // A
    char **attributes_value; // A
    char **child_name;       // A
    char **child_sPtr;       // A
    char **child_ePtr;       // A
};


// Function Declarations
struct XML_Document *xml_load_document(char *filepath);
struct XML_RootNode *xml_get_root_node(struct XML_Document *doc, const char root_name[]);
struct XML_ChildNode *xml_get_child_node(int child_num, struct XML_Document *doc, struct XML_RootNode *root, struct XML_ChildNode *node);
struct XML_Attributes *xml_get_attributes(char *name, char *sPtr, char *ePtr);
char *xml_get_value(struct XML_Document *doc, struct XML_ChildNode *node, char aKey[], char aVal[], char cName[]);
int xml_is_simple(char *sPtr, char *ePtr, char *name);
void xml_extract_attributes(int name_len, char *sPtr, struct XML_RootNode *root, struct XML_ChildNode *node);
void xml_free_doc(struct XML_Document *doc);
void xml_free_attr(struct XML_Attributes *attr);
void xml_free_root(struct XML_RootNode *root);
void xml_free_child(struct XML_ChildNode *child);

// Function Definitions

/**
 * xml_load_document
 * Takes in the file path
 * Returns struct containing file info and data as well
 * Need to free in the end by calling XML_DocFree() Function 
 */
struct XML_Document *xml_load_document(char *filepath) {
    FILE *xml_file;
    struct XML_Document *doc;

    doc = malloc(sizeof(struct XML_Document));
    doc->filepath = malloc(strlen(filepath)+1);
    strcpy(doc->filepath, filepath);
    xml_file = fopen(filepath, "rb");
    
    fseek(xml_file, 0, SEEK_END);
    doc->len = ftell(xml_file);
    doc->data = calloc(doc->len+1, 1);   // Account for null byte ('\0')
    rewind(xml_file);
    fread(doc->data, 1, doc->len, xml_file);
    
    fclose(xml_file);
    return doc;
}

// Pointer point to '<', not the node name, so consider for +1
struct XML_RootNode *xml_get_root_node(struct XML_Document *doc, const char root_name[]) {
    struct XML_RootNode *root;
    int name_len, depth, temp, n, t;
    int temp_child_name_len, temp_num_childs;
    bool root_node_found;

    root = malloc(sizeof(struct XML_RootNode));
    name_len = strlen(root_name);
    root_node_found = false;
    temp_num_childs = 0;
    root->success = 0;
    root->has_attributes = false;
    root->has_spaces = false;
    root->has_crlf = true; // Mostly line ends with \r\n
    root->num_attributes = 0;
    root->num_childs = 0;
    root->num_sep = 0;
    root->child_sPtr = NULL;
    root->child_ePtr = NULL;
    root->child_name = NULL;
    root->child_isSimple = NULL;
    root->attributes_key = NULL;
    root->attributes_value = NULL;
    depth = 0;

    for (n = 0; n < doc->len; n++) {
        if ((doc->data[n] == '<')) {
            
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
            }
            
            // Finding the root nodes' starting pointer
            if ((memcmp((doc->data+n+1), root_name, name_len) == 0) && ((doc->data[n+name_len+1] == ' ') || (doc->data[n+name_len+1] == '>'))) { // Nested if for better readability
                if ((depth == 0) && (root_node_found == false)) {
                    // Copying root sPtr and root name here
                    root->root_sPtr = doc->data+n; // Assigning the start pointer of the root node in the data array
                    root->root_name_len = name_len;
                    root->root_name = calloc(name_len+1, 1);
                    memcpy(root->root_name, (doc->data+n+1), name_len);

                    // Checking for attributes here
                    if ((doc->data[n+name_len+1] == ' ') && (root->has_attributes == 0)) {
                        xml_extract_attributes(root->root_name_len, root->root_sPtr, root, NULL);
                    }

                    while (doc->data[n] != '>') {
                        n++;
                    }

                    if (doc->data[n+1] == '\r' && doc->data[n+2] == '\n') {
                        n += 2;
                        root->has_crlf = true;
                    } else if (doc->data[n+1] == '\n') {
                        n++;
                        root->has_crlf = false;
                    }

                    if (doc->data[n+1] == ' ') {
                        root->has_spaces = true;
                    } else if (doc->data[n+1] == '\t') {
                        root->has_spaces = false;
                    }

                    n++; // To go the space or the tab character.
                    while (doc->data[n] != '<') {
                        root->num_sep++;
                        n++;
                    }
                    n--; // Because for loop increments it above, thus skipping first child
                    //n = n + name_len + 1;               // Changing value of n, so that it does not need to iterate over the root_name
                    depth = 1;
                    root_node_found = true;
                    continue;
                } else {
                    if (depth != 0) {
                        root->success == 1002; // Root node found twice
                    } else if (root_node_found != false) {
                        root->success == 1003; // root node have already been found, doesn't make sense for it to be here->
                    }
                    break;
                }
            }

            if (root_node_found) {
                if (depth == 1) {
                    if ((doc->data[n+1] == '/')) {
                        if (memcmp((doc->data + n + 2), root->root_name, root->root_name_len) == 0) {
                            root->root_ePtr = doc->data+n;
                            root->success = 1000; // Successfully parsed the xml file
                            break;
                        } else {
                            root->success == 1004; // Root Node not terminating correctly
                        }
                    } else {
                        for (t=n+1; t<doc->len; t++) {
                            if ((doc->data[t] == ' ') || (doc->data[t] == '>')) {
                                temp_child_name_len = t-n-1;
                                root->child_name = realloc(root->child_name, (temp_num_childs+1)*sizeof(char *));
                                root->child_name[temp_num_childs] = calloc(temp_child_name_len + 1, 1);
                                memcpy(root->child_name[temp_num_childs], (doc->data + n + 1), temp_child_name_len);
                                
                                if (root->child_sPtr == NULL) {
                                    root->child_sPtr = malloc((temp_num_childs + 1) * sizeof(char *));
                                    root->child_sPtr[temp_num_childs] = (doc->data + n);
                                } else {
                                    root->child_sPtr = realloc(root->child_sPtr, (temp_num_childs + 1) * sizeof(char *));
                                    root->child_sPtr[temp_num_childs] = (doc->data + n);
                                }
                                temp = t+1;
                                depth++;
                                break;
                            }
                        }
                        n = temp;
                        continue;
                    }
                } else {
                    if (doc->data[n+1] == '/') {
                        if ((memcmp((doc->data+n+2), root->child_name[temp_num_childs], temp_child_name_len) == 0) && (doc->data[n+2+temp_child_name_len] == '>')) {
                            // Setting child end pointer
                            if (root->child_ePtr == NULL) {
                                root->child_ePtr = malloc((temp_num_childs + 1) * sizeof(char *));
                                root->child_ePtr[temp_num_childs] = (doc->data + n);
                            } else {
                                root->child_ePtr = realloc(root->child_ePtr, (temp_num_childs + 1) * sizeof(char *));
                                root->child_ePtr[temp_num_childs] = (doc->data + n);
                            }

                            // Checking child is simple or not and setting child name
                            int simple = xml_is_simple(root->child_sPtr[temp_num_childs], root->child_ePtr[temp_num_childs], root->child_name[temp_num_childs]);
                            root->child_isSimple = realloc(root->child_isSimple, (temp_num_childs + 1) * sizeof(int));
                            if (simple) {
                                root->child_isSimple[temp_num_childs] = true;
                            } else {
                                root->child_isSimple[temp_num_childs] = false;
                            }

                            // finalising child related stuff
                            n = n + temp_child_name_len + 2;
                            temp_num_childs++;
                            root->num_childs = temp_num_childs;
                            depth--;
                        }
                    }
                }
            }
        }
    }

    root->depth = depth;
    return root;
}

struct XML_ChildNode *xml_get_child_node(int child_num, struct XML_Document *doc, struct XML_RootNode *root, struct XML_ChildNode *node) {
    
    struct XML_ChildNode *new_node;
    int name_len, depth, temp, temp_n, n, t;
    int temp_child_name_len, temp_num_childs;

    if (!node) {
        name_len = strlen(root->child_name[child_num]);
        depth = root->depth + 1;
    } else if (!root) {
        name_len = strlen(node->child_name[child_num]);
        depth = node->depth + 1;
    }
    
    new_node = malloc(sizeof(struct XML_ChildNode));
    temp_num_childs = 0;
    temp_child_name_len = 0;
    new_node->success = 0;
    new_node->has_attributes = false;
    new_node->has_spaces = false;
    new_node->has_crlf = true; // Mostly lines end with \r\n
    new_node->num_attributes = 0;
    new_node->num_childs = 0;
    new_node->child_sPtr = NULL;
    new_node->child_ePtr = NULL;
    new_node->child_name = NULL;
    new_node->child_isSimple = NULL;
    new_node->attributes_key = NULL;
    new_node->attributes_value = NULL;


    if (root) {
        
        temp_n = (root->child_sPtr[child_num] - doc->data + name_len);
        new_node->node_name_len = name_len;
        new_node->node_name = calloc((new_node->node_name_len + 1), 1);
        memcpy(new_node->node_name, root->child_name[child_num], new_node->node_name_len);
        new_node->node_sPtr = root->child_sPtr[child_num];
        new_node->node_ePtr = root->child_ePtr[child_num];
        new_node->has_spaces = root->has_spaces;
        new_node->num_sep = root->num_sep;

        for (n = temp_n; n < doc->len; n++) {
            if (depth == (root->depth + 1)) {
                if ((doc->data[n] == ' ') && (new_node->has_attributes == 0)) {
                    xml_extract_attributes(new_node->node_name_len, new_node->node_sPtr, NULL, new_node);
                } else if ((doc->data+n) == root->child_ePtr[child_num]) {
                    // Ending pointer, or actually much better, use the child_eptr, otherwise what is its use is gonna be
                    new_node->success = 1000; // Basically the whole child and stuff parsed.
                    break;
                } else if ((doc->data[n] == '<') && (doc->data[n+1] != '/')) {
                    // Depth will change because child encountered
                    for (t=n+1; t<doc->len; t++) {
                        if ((doc->data[t] == ' ') || (doc->data[t] == '>')) {
                            temp_child_name_len = t-n-1;
                            new_node->child_name = realloc(new_node->child_name, (temp_num_childs+1)*sizeof(char *));
                            new_node->child_name[temp_num_childs] = calloc(temp_child_name_len + 1, 1);
                            memcpy(new_node->child_name[temp_num_childs], (doc->data + n + 1), temp_child_name_len);
                            
                            if (new_node->child_sPtr == NULL) {
                                new_node->child_sPtr = malloc((temp_num_childs + 1) * sizeof(char *));
                                new_node->child_sPtr[temp_num_childs] = (doc->data + n);
                            } else {
                                new_node->child_sPtr = realloc(new_node->child_sPtr, (temp_num_childs + 1) * sizeof(char *));
                                new_node->child_sPtr[temp_num_childs] = (doc->data + n);
                            }
                            temp = t+1;
                            depth++;
                            break;
                        }
                    }
                    n = temp;
                    continue;
                }
            } else {
                // Child stuff here
                if ((doc->data[n] == '<') && (doc->data[n+1] == '/')) {
                    if ((memcmp((doc->data+n+2), new_node->child_name[temp_num_childs], temp_child_name_len) == 0) && (doc->data[n+2+temp_child_name_len] == '>')) {
                        // Setting child end pointer
                        if (new_node->child_ePtr == NULL) {
                            new_node->child_ePtr = malloc((temp_num_childs + 1) * sizeof(char *));
                            new_node->child_ePtr[temp_num_childs] = (doc->data + n);
                        } else {
                            new_node->child_ePtr = realloc(new_node->child_ePtr, (temp_num_childs + 1) * sizeof(char *));
                            new_node->child_ePtr[temp_num_childs] = (doc->data + n);
                        }

                        // Checking child is simple or not and setting child name
                        int simple = xml_is_simple(new_node->child_sPtr[temp_num_childs], new_node->child_ePtr[temp_num_childs], new_node->child_name[temp_num_childs]);
                        new_node->child_isSimple = realloc(new_node->child_isSimple, (temp_num_childs + 1) * sizeof(int));
                        if (simple) {
                            new_node->child_isSimple[temp_num_childs] = true;
                        } else {
                            new_node->child_isSimple[temp_num_childs] = false;
                        }

                        // finalising child related stuff
                        n = n + temp_child_name_len + 2;
                        temp_num_childs++;
                        new_node->num_childs = temp_num_childs;
                        depth--;
                    }
                }
            }
        }
    } else if (node) {
        temp_n = (node->child_sPtr[child_num] - doc->data + name_len);
        new_node->node_name_len = name_len;
        new_node->node_name = calloc((new_node->node_name_len + 1), 1);
        memcpy(new_node->node_name, node->child_name[child_num], new_node->node_name_len);
        new_node->node_sPtr = node->child_sPtr[child_num];
        new_node->node_ePtr = node->child_ePtr[child_num];
        new_node->has_spaces = node->has_spaces;
        new_node->num_sep = node->num_sep;

        for (n = temp_n; n < doc->len; n++) {
            if (depth == (node->depth + 1)) {
                if ((doc->data[n] == ' ') && (new_node->has_attributes == 0)) {
                    xml_extract_attributes(new_node->node_name_len, new_node->node_sPtr, NULL, new_node);
                } else if ((doc->data+n) == node->child_ePtr[child_num]) {
                    // Ending pointer, or actually much better, use the child_eptr, otherwise what is its use is gonna be
                    new_node->success = 1000; // Basically the whole child and stuff parsed.
                    break;
                } else if ((doc->data[n] == '<') && (doc->data[n+1] != '/')) {
                    // Depth will change because child encountered
                    for (t=n+1; t<doc->len; t++) {
                        if ((doc->data[t] == ' ') || (doc->data[t] == '>')) {
                            temp_child_name_len = t-n-1;
                            new_node->child_name = realloc(new_node->child_name, (temp_num_childs+1)*sizeof(char *));
                            new_node->child_name[temp_num_childs] = calloc(temp_child_name_len + 1, 1);
                            memcpy(new_node->child_name[temp_num_childs], (doc->data + n + 1), temp_child_name_len);
                            
                            if (new_node->child_sPtr == NULL) {
                                new_node->child_sPtr = malloc((temp_num_childs + 1) * sizeof(char *));
                                new_node->child_sPtr[temp_num_childs] = (doc->data + n);
                            } else {
                                new_node->child_sPtr = realloc(new_node->child_sPtr, (temp_num_childs + 1) * sizeof(char *));
                                new_node->child_sPtr[temp_num_childs] = (doc->data + n);
                            }
                            temp = t+1;
                            depth++;
                            break;
                        }
                    }
                    n = temp;
                    continue;
                }
            } else {
                // Child stuff here
                if ((doc->data[n] == '<') && (doc->data[n+1] == '/')) {
                    if ((memcmp((doc->data+n+2), new_node->child_name[temp_num_childs], temp_child_name_len) == 0) && (doc->data[n+2+temp_child_name_len] == '>')) {
                        // Setting child end pointer
                        if (new_node->child_ePtr == NULL) {
                            new_node->child_ePtr = malloc((temp_num_childs + 1) * sizeof(char *));
                            new_node->child_ePtr[temp_num_childs] = (doc->data + n);
                        } else {
                            new_node->child_ePtr = realloc(new_node->child_ePtr, (temp_num_childs + 1) * sizeof(char *));
                            new_node->child_ePtr[temp_num_childs] = (doc->data + n);
                        }

                        // Checking child is simple or not and setting child name
                        int simple = xml_is_simple(new_node->child_sPtr[temp_num_childs], new_node->child_ePtr[temp_num_childs], new_node->child_name[temp_num_childs]);
                        new_node->child_isSimple = realloc(new_node->child_isSimple, (temp_num_childs + 1) * sizeof(int));
                        if (simple) {
                            new_node->child_isSimple[temp_num_childs] = true;
                        } else {
                            new_node->child_isSimple[temp_num_childs] = false;
                        }

                        // finalising child related stuff
                        n = n + temp_child_name_len + 2;
                        temp_num_childs++;
                        new_node->num_childs = temp_num_childs;
                        depth--;
                    }
                }
            }
        }
    }

    new_node->depth = depth;
    return new_node;
}

char *xml_get_value(struct XML_Document *doc, struct XML_ChildNode *node, char aKey[], char aVal[], char cName[]) {
    int n, o, num_indexes, nameLen, dataLen;
    bool node_found;
    char *data, *sPtr, *ePtr;
    struct XML_Attributes *attr;

    num_indexes = 0;
    node_found = false;
    attr = NULL;

    for (n = 0; n < node->num_childs; n++) {
        if (strcmp(node->child_name[n], cName) == 0) {
            if (aKey && aVal) {
                attr = xml_get_attributes(cName, node->child_sPtr[n], node->child_ePtr[n]);

                for (o = 0; o < attr->numA; o++) {
                    if ((strcmp(attr->aKey[o], aKey) == 0) && (strcmp(attr->aVal[o], aVal) == 0)) {
                        node_found = true;
                        break;
                    }
                }
                
                if (!node_found) {
                    xml_free_attr(attr);
                    continue;
                }
                sPtr = node->child_sPtr[n];
                ePtr = node->child_ePtr[n];
                break;
            } else {
                node_found = true;
                sPtr = node->child_sPtr[n];
                ePtr = node->child_ePtr[n];
                break;
            }
        }
    }

    if (node_found) {
        n = 0;

        while (sPtr[n] != '>') {
            n++;
        }

        n++; // To go to the starting character.
        data = calloc(ePtr - (sPtr + n) + 1, 1); // +1 For \0
        memcpy(data, (sPtr + n), (ePtr - (sPtr+n)));
        if (attr) {
            xml_free_attr(attr);
        }

        return data;
    } else {
        return NULL;
    }

}

void xml_free_doc(struct XML_Document *doc) {
    free(doc->data);
    free(doc->filepath);
    free(doc);
}

void xml_free_attr(struct XML_Attributes *attr) {
    for (int n = 0; n < attr->numA; n++) {
        free(attr->aKey[n]);
        free(attr->aVal[n]);
    }
    free(attr);
}

void xml_free_root(struct XML_RootNode *root) {
    int i;

    for (i = 0; i < root->num_childs; i++) {
        free(root->child_name[i]);
    }
    
    for (i = 0; i < root->num_attributes; i++) {
        free(root->attributes_key[i]);
        free(root->attributes_value[i]);
    }

    free(root->child_sPtr);
    free(root->child_ePtr);
    free(root->child_isSimple);
    free(root->root_name);
    free(root);
}

void xml_free_child(struct XML_ChildNode *child) {
    int i;

    for (i = 0; i < child->num_childs; i++) {
        free(child->child_name[i]);
    }
    
    for (i = 0; i < child->num_attributes; i++) {
        free(child->attributes_key[i]);
        free(child->attributes_value[i]);
    }

    free(child->child_sPtr);
    free(child->child_ePtr);
    free(child->child_isSimple);
    free(child->node_name);
    free(child);
}

int xml_is_simple(char *sPtr, char *ePtr, char *name) {
    size_t len;
    int is_simple, n;
    char *ptr;
    char *eptr;

    is_simple = true; // Assuming it is simple;
    ptr = (sPtr + strlen(name) + 1); // removing the starting bracket of the node. Example "<root>abc</root> == abc</root>"
    n = 0;

    if (ptr[0] == ' ') {
        while (ptr[n] != '>') {
            n++;
        }
    }

    ptr = ptr + n + 1;
    len = ePtr - ptr; // subtracting name length for the starting tag and subtraction 2 for the angular brackets.

    for (n = 0; n < len; n++) {
        if ((ptr[n] == '<') || (ptr[n] == '>')) {
            is_simple = false;
            break;
        } 
    }

    return is_simple;
}

void xml_extract_attributes(int name_len, char *sPtr, struct XML_RootNode *root, struct XML_ChildNode *node) {
    int attr_key_len, attr_value_len, ampersand_count, n;

    n = name_len + 2; // +2 : 1 for '<' and the other one for the space
    attr_key_len = 0;
    attr_value_len = 0;
    ampersand_count = 0;

    while (sPtr[n] != '>') {
        if (root) { // Root Node
            // Getting the key length here
            while ((sPtr[n] != '=') && (sPtr[n] != ' ')) {
                attr_key_len++;
                n++;
            }

            // Adding the key to the node
            root->attributes_key = realloc(root->attributes_key, root->num_attributes * sizeof(char *));
            root->attributes_key[root->num_attributes] = calloc(attr_key_len + 1, 1);
            memcpy(root->attributes_key[root->num_attributes], (sPtr + n - attr_key_len), attr_key_len);

            // Incrementing n until '"' found
            while (sPtr[n] != '"') {
                n++;
            }

            // Getting the length of value here
            while(ampersand_count != 2) {
                if (sPtr[n] == '"') {
                    ampersand_count++;
                } else {
                    attr_value_len++;
                }
                n++;
            }

            // Adding the value to the node
            root->attributes_value = realloc(root->attributes_value, root->num_attributes * sizeof(char *));
            root->attributes_value[root->num_attributes] = calloc(attr_value_len + 1, 1);
            memcpy(root->attributes_value[root->num_attributes], (sPtr + n - attr_value_len - 1), attr_value_len);
            root->num_attributes++;

            while (sPtr[n] == ' ') {
                n++;
            }
        } else if (node) { // Child Nodes
            // Getting the key length here
            while ((sPtr[n] != '=') && (sPtr[n] != ' ')) {
                attr_key_len++;
                n++;
            }

            // Adding the key to the node
            node->attributes_key = realloc(node->attributes_key, node->num_attributes * sizeof(char *));
            node->attributes_key[node->num_attributes] = calloc(attr_key_len + 1, 1);
            memcpy(node->attributes_key[node->num_attributes], (sPtr + n - attr_key_len), attr_key_len);

            // Incrementing n until '"' found
            while (sPtr[n] != '"') {
                n++;
            }

            // Getting the length of value here
            while(ampersand_count != 2) {
                if (sPtr[n] == '"') {
                    ampersand_count++;
                } else {
                    attr_value_len++;
                }
                n++;
            }

            // Adding the value to the node
            node->attributes_value = realloc(node->attributes_value, node->num_attributes * sizeof(char *));
            node->attributes_value[node->num_attributes] = calloc(attr_value_len + 1, 1);
            memcpy(node->attributes_value[node->num_attributes], (sPtr + n - attr_value_len - 1), attr_value_len);
            node->num_attributes++;

            while (sPtr[n] == ' ') {
                n++;
            }
        }

        // Setting the control Parameters back to 0
        attr_key_len = 0;
        attr_value_len = 0;
        ampersand_count = 0;
    }

    if (root) {
        root->has_attributes = 1;
    } else if (node) {
        node->has_attributes = 1;
    }
}

struct XML_Attributes *xml_get_attributes(char *name, char *sPtr, char *ePtr) {
    int n, nodeL;
    int attr_key_len, attr_value_len, ampersand_count;

    nodeL = ePtr - sPtr;
    n = strlen(name) + 2;
    struct XML_Attributes *attr = malloc(sizeof(struct XML_Attributes));
    attr->numA = 0;
    attr->aKey = NULL;
    attr->aVal = NULL;

    attr_key_len = 0;
    attr_value_len = 0;
    ampersand_count = 0;

    if (sPtr[n + 1] == '>') {
        return attr;
    }

    while (sPtr[n] != '>') {
        // Getting the key length here
        while ((sPtr[n] != '=') && (sPtr[n] != ' ')) {
            attr_key_len++;
            n++;
        }

        // Adding the key to the node
        attr->aKey = realloc(attr->aKey, (attr->numA + 1) * sizeof(char *));
        attr->aKey[attr->numA] = calloc(attr_key_len + 1, 1);
        memcpy(attr->aKey[attr->numA], (sPtr + n - attr_key_len), attr_key_len);

        // Incrementing n until '"' found
        while (sPtr[n] != '"') {
            n++;
        }

        // Getting the length of value here
        while(ampersand_count != 2) {
            if (sPtr[n] == '"') {
                ampersand_count++;
            } else {
                attr_value_len++;
            }
            n++;
        }

        // Adding the value to the node
        attr->aVal = realloc(attr->aVal, (attr->numA + 1) * sizeof(char *));
        attr->aVal[attr->numA] = calloc(attr_value_len + 1, 1);
        memcpy(attr->aVal[attr->numA], (sPtr + n - attr_value_len - 1), attr_value_len);
        attr->numA++;

        while (sPtr[n] == ' ') {
            n++;
        }

        attr_key_len = 0;
        attr_value_len = 0;
        ampersand_count = 0;
    }
    return attr;
}