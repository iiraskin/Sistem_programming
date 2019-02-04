DEF_OP(ADD, '+', 0, {
    FormulaNode* new_node = new FormulaNode(false, OP_ADD);
    new_node->left = DiffTree(_root->left, f);
    new_node->right = DiffTree(_root->right, f);
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (!_root->right->IsLeaf() && _root->right->value == COM_MINUS) {
        _root->value = OP_SUB;
        FormulaNode* new_right = _root->right->left;
        delete _root->right;
        _root->right = new_right;
        OptimiseTree(_root);
        return;
    }
    if (!_root->left->IsLeaf() && _root->left->value == COM_MINUS) {
        _root->value = OP_SUB;
        FormulaNode* new_right = _root->left->left;
        delete _root->left;
        _root->left = _root->right;
        _root->right = new_right;
        OptimiseTree(_root);
        return;
    }
    if (_root->left->IsLeaf() && _root->left->is_number_or_func &&
        _root->right->IsLeaf() && _root->right->is_number_or_func) {
        FormulaNode* new_root = _root->left;
        new_root->value += _root->right->value;
        delete _root;
        delete _root->right;
        _root = new_root;
        return;
    }
    if (IsZero(_root->left)) {
        FormulaNode* new_root = _root->right;
        delete _root;
        delete _root->left;
        _root = new_root;
        return;
    }
    if (IsZero(_root->right)) {
        FormulaNode* new_root = _root->left;
        delete _root;
        delete _root->right;
        _root = new_root;
        return;
    }
    return;})

DEF_OP(SUB, '-', 1, {
    FormulaNode* new_node = new FormulaNode(false, OP_SUB);
    new_node->left = DiffTree(_root->left, f);
    new_node->right = DiffTree(_root->right, f);
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (!_root->right->IsLeaf() && _root->right->value == COM_MINUS) {
        _root->value = OP_ADD;
        FormulaNode* new_right = _root->right->left;
        delete _root->right;
        _root->right = new_right;
        OptimiseTree(_root);
        return;
    }
    if (!_root->left->IsLeaf() && _root->left->value == COM_MINUS) {
        _root->value = OP_ADD;
        FormulaNode* new_root = _root->left;
        _root->left = _root->left->left;
        new_root->left = _root;
        _root = new_root;
        OptimiseTree(_root->left);
        return;
    }
    if (_root->left->IsLeaf() && _root->left->is_number_or_func &&
        _root->right->IsLeaf() && _root->right->is_number_or_func) {
        FormulaNode* new_root = _root->left;
        new_root->value -= _root->right->value;
        delete _root;
        delete _root->right;
        _root = new_root;
        if (_root->value < 0) {
            _root->value = - _root->value;
            FormulaNode* new_root = new FormulaNode(true, COM_MINUS);
            new_root->left = _root;
            _root = new_root;
        }
        return;
    }
    if (IsZero(_root->left)) {
        FormulaNode* new_left = _root->right;
        delete _root;
        delete _root->left;
        FormulaNode* new_root = new FormulaNode(true, COM_MINUS);
        new_root->left = new_left;
        _root = new_root;
        return;
    }
    if (IsZero(_root->right)) {
        FormulaNode* new_root = _root->left;
        delete _root;
        delete _root->right;
        _root = new_root;
        return;
    }
    return;})

DEF_OP(MUL, '*', 2, {
    FormulaNode* new_node = new FormulaNode(false, OP_ADD);
    FormulaNode* new_node_l = new FormulaNode(false, OP_MUL);
    FormulaNode* new_node_r = new FormulaNode(false, OP_MUL);
    new_node_l->left = DiffTree(_root->left, f);
    new_node_l->right = _root->right->Copy();
    new_node_r->left = _root->left->Copy();
    new_node_r->right = DiffTree(_root->right, f);
    new_node->left = new_node_l;
    new_node->right = new_node_r;
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},

    {if (!_root->right->IsLeaf() && _root->right->value == COM_MINUS) {
        FormulaNode* new_root = _root->right;
        _root->right = new_root->left;
        new_root->left = _root;
        _root = new_root;
        OptimiseTree(_root->left);
        return;
    }
    if (!_root->left->IsLeaf() && _root->left->value == COM_MINUS) {
        FormulaNode* new_root = _root->left;
        _root->left = new_root->left;
        new_root->left = _root;
        _root = new_root;
        OptimiseTree(_root->left);
        return;
    }
    if (_root->left->IsLeaf() && _root->left->is_number_or_func &&
        _root->right->IsLeaf() && _root->right->is_number_or_func) {
        FormulaNode* new_root = _root->left;
        new_root->value *= _root->right->value;
        delete _root;
        delete _root->right;
        _root = new_root;
        return;
    }
    if (IsZero(_root->left) || IsOne(_root->right)) {
        DeleteTree(_root->right);
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
        return;
    }
    if (IsZero(_root->right) || IsOne(_root->left)) {
        DeleteTree(_root->left);
        FormulaNode* new_root = _root->right;
        delete _root;
        _root = new_root;
        return;
    }
    return;})

DEF_OP(DIV, '/', 3, {
    FormulaNode* new_node = new FormulaNode(false, OP_DIV);
    FormulaNode* new_node_l = new FormulaNode(false, OP_SUB);
    FormulaNode* new_node_l_l = new FormulaNode(false, OP_MUL);
    FormulaNode* new_node_l_r = new FormulaNode(false, OP_MUL);
    FormulaNode* new_node_r = new FormulaNode(false, OP_POW);
    new_node_l_l->left = DiffTree(_root->left, f);
    new_node_l_l->right = _root->right->Copy();
    new_node_l_r->left = _root->left->Copy();
    new_node_l_r->right = DiffTree(_root->right, f);
    new_node_l->left = new_node_l_l;
    new_node_l->right = new_node_l_r;
    new_node_r->left = _root->right->Copy();
    new_node_r->right = new FormulaNode(true, 2);
    new_node->left = new_node_l;
    new_node->right = new_node_r;
    if (f) {
        TexDiff(f, _root, new_node);
    }
    return new_node;},
    {if (!_root->right->IsLeaf() && _root->right->value == COM_MINUS) {
        FormulaNode* new_root = _root->right;
        _root->right = new_root->left;
        new_root->left = _root;
        _root = new_root;
        OptimiseTree(_root->left);
        return;
    }
    if (!_root->left->IsLeaf() && _root->left->value == COM_MINUS) {
        FormulaNode* new_root = _root->left;
        _root->left = new_root->left;
        new_root->left = _root;
        _root = new_root;
        OptimiseTree(_root->left);
        return;
    }
    if (_root->left->IsLeaf() && _root->left->is_number_or_func &&
        _root->right->IsLeaf() && _root->right->is_number_or_func &&
        _root->right->value != 0 && _root->left->value % _root->right->value == 0) {
        FormulaNode* new_root = _root->left;
        new_root->value /= _root->right->value;
        delete _root;
        delete _root->right;
        _root = new_root;
        return;
    }
    if (IsZero(_root->left) || IsOne(_root->right)) {
        DeleteTree(_root->right);
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
        return;
    }
    if (IsZero(_root->right)) {
        printf("Warning: division by 0.\n");
        return;
    }
    return;})

DEF_OP(POW, '^', 4, {
    if (_root->left->IsLeaf() && _root->left->is_number_or_func) {
        FormulaNode* new_node = new FormulaNode(false, OP_SUB);
        FormulaNode* new_node_l = new FormulaNode(false, OP_SUB);
        new_node_l->left = _root->left->Copy();
        FormulaNode* new_node_l_r = new FormulaNode(false, COM_LOG);
        new_node_l_r = _root->left->Copy();
        new_node_l->right = new_node_l_r;
        new_node->left = new_node_l;
        new_node->right = DiffTree(_root->right, f);
        if (f) {
            TexDiff(f, _root, new_node);
        }
        return new_node;
    }
    if (_root->right->IsLeaf() && _root->right->is_number_or_func) {
        FormulaNode* new_node = new FormulaNode(false, OP_MUL);
        FormulaNode* new_node_l = new FormulaNode(false, OP_MUL);
        new_node_l->left = new FormulaNode(true, _root->right->value);
        new_node_l->right = _root->Copy();
        new_node_l->right->right->value -= 1;
        new_node->left = new_node_l;
        new_node->right = DiffTree(_root->left, f);
        if (f) {
            TexDiff(f, _root, new_node);
        }
        return new_node;
    }
    printf("CFormulaTree::DiffTree: This program can not differentiate f(x) ^ f(x).\n");
    return nullptr;},
    {if (IsZero(_root->left) ||
        (_root->left->IsLeaf() && _root->left->is_number_or_func && _root->left->value == 1)) {
        DeleteTree(_root->right);
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
        return;
    }
    if (IsZero(_root->right)) {
        DeleteTree(_root->left);
        FormulaNode* new_root = _root->right;
        delete _root;
        _root = new_root;
        _root->value = 1;
        return;
    }
    if (_root->right->IsLeaf() && _root->right->is_number_or_func && _root->right->value == 1) {
        delete _root->right;
        FormulaNode* new_root = _root->left;
        delete _root;
        _root = new_root;
        return;
    }
    return;})
