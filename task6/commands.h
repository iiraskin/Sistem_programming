DEF_COM(MINUS, "-", 10, {
    FormulaNode* new_node = new FormulaNode(true, COM_MINUS);
    new_node->left = DiffTree(_root->left, f);
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (_root->left->value == COM_MINUS) {
        FormulaNode* new_root = _root->left->left;
        delete _root->left;
        delete _root;
        _root = new_root;
    } else if (IsZero(_root->left)) {
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
    }
    return;})

DEF_COM(SIN, "sin", 11, {
    FormulaNode* new_node = new FormulaNode(false, OP_MUL);
    FormulaNode* new_node_r = new FormulaNode(true, COM_COS);
    new_node_r->left = _root->left->Copy();
    new_node->left = DiffTree(_root->left, f);
    new_node->right = new_node_r;
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (IsZero(_root->left)) {
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
    }
    return;})

DEF_COM(COS, "cos", 12, {
    FormulaNode* new_node = new FormulaNode(true, COM_MINUS);
    FormulaNode* new_node_l = new FormulaNode(false, OP_MUL);
    FormulaNode* new_node_l_r = new FormulaNode(true, COM_SIN);
    new_node_l_r->left = _root->left->Copy();
    new_node_l->left = DiffTree(_root->left, f);
    new_node_l->right = new_node_l_r;
    new_node->left = new_node_l;
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (IsZero(_root->left)) {
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
        _root->value = 1;
    }
    return;})

DEF_COM(TAN, "tan", 13, {
    FormulaNode* new_node = new FormulaNode(false, OP_DIV);
    FormulaNode* new_node_r = new FormulaNode(false, OP_POW);
    FormulaNode* new_node_r_l = new FormulaNode(true, COM_COS);
    FormulaNode* new_node_r_r = new FormulaNode(true, 2);
    new_node_r_l->left = _root->left->Copy();
    new_node_r->left = new_node_r_l;
    new_node_r->right = new_node_r_r;
    new_node->left = DiffTree(_root->left, f);
    new_node->right = new_node_r;
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (IsZero(_root->left)) {
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
    }
    return;})

DEF_COM(COT, "cot", 14, {
    FormulaNode* new_node = new FormulaNode(true, COM_MINUS);
    FormulaNode* new_node_l = new FormulaNode(false, OP_DIV);
    FormulaNode* new_node_l_r = new FormulaNode(false, OP_POW);
    FormulaNode* new_node_l_r_l = new FormulaNode(true, COM_SIN);
    FormulaNode* new_node_l_r_r = new FormulaNode(true, 2);
    new_node_l_r_l->left = _root->left->Copy();
    new_node_l_r->left = new_node_l_r_l;
    new_node_l_r->right = new_node_l_r_r;
    new_node_l->left = DiffTree(_root->left, f);
    new_node_l->right = new_node_l_r;
    new_node->left = new_node_l;
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (IsZero(_root->left)) {
        printf("Warning: division by 0.\n");
    }
    return;})

DEF_COM(EXP, "exp", 15, {
    FormulaNode* new_node = new FormulaNode(false, OP_MUL);
    new_node->left = DiffTree(_root->left, f);
    new_node->right = _root->Copy();
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (IsZero(_root->left)) {
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
        _root->value = 1;
    }
    return;})

DEF_COM(LOG, "log", 16, {
    FormulaNode* new_node = new FormulaNode(false, OP_DIV);
    new_node->left = DiffTree(_root->left, f);
    new_node->right = _root->left->Copy();
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (IsZero(_root->left)) {
        printf("Warning: logarithm of 0.\n");
    }
    return;})

