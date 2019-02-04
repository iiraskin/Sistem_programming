DEF_CMD(JUMP, 14, "jump", JumpHandling(i, labels, comm_buff[i]), JumpHandling(int &i, std::unordered_map<int, int>& labels, int cmd),
    {comm_buff.push_back(CMD_JUMP);

    std::string lab = {};

    if (ReadLabel(i, commands, commands_len, lab)) {
        PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ReadLabel\n")
    }

    if (labels.find(lab) == labels.end()) {
        labels.insert({lab, i});
        comm_buff.push_back(i);
    } else {
        comm_buff.push_back(labels[lab]);
    }},

    {CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command jump (no argument)\n")
    fprintf(fd, "jump %d\n", comm_buff[++i]);})

DEF_CMD(PUSH, 7, "push", PushHandling(i), PushHandling(int& i),
    {SKIP_SPACES

    CHECK_COMMS_END("CRPNHandler::ParserWords: Command 'push' does not has an argument\n")

    if (commands[i] == 'r') {
        comm_buff.push_back(CMD_PUSHR);
        if (ParseRegister(i, commands, commands_len)) {
            PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ParseRegister\n")
        }
    } else if (commands[i] == '[') {
        comm_buff.push_back(CMD_PUSHRAM);
        ++i;

        if (ParseNumber(i, commands, commands_len)) {
            PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ParseNumber\n")
        }

        SKIP_SPACES

        if (commands[i] != ']') {
            PARSER_ERROR("CRPNHandler::ParserWords: Incorrect syntax\n")
        }
        ++i;
    } else {
        comm_buff.push_back(CMD_PUSH);
        if (ParseNumber(i, commands, commands_len)) {
            PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ParseNumber\n")
        }
    }},

    {CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command push (no argument)\n")
    fprintf(fd, "push %d\n", comm_buff[++i]);})

DEF_CMD(POPR, 9, "pop", PopRHandling(i), PopRHandling(int& i),
    {SKIP_SPACES
    CHECK_COMMS_END("CRPNHandler::ParserWords: Command 'push' does not has an argument\n")

    if (commands[i] == 'r') {
        comm_buff.push_back(CMD_POPR);
        if (ParseRegister(i, commands, commands_len)) {
            PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ParseRegister\n")
        }
    } else if (commands[i] == '[') {
        comm_buff.push_back(CMD_POPRAM);
        ++i;
        if (ParseNumber(i, commands, commands_len)) {
            PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ParseNumber\n")
        }

        SKIP_SPACES
        if (commands[i] != ']') {
            PARSER_ERROR("CRPNHandler::ParserWords: Incorrect syntax\n")
        }
        ++i;
    } else {
        PARSER_ERROR("CRPNHandler::ParserWords: Incorrect argument\n")
    }},

    {fprintf(fd, "pop ");
    CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command pop (no argument)\n")

    if (PrintRegister(fd, i)) {
        PARSER_ERROR("CRPNHandler::PrintWords: Error in CRPNHandler::PrintRegister\n")
    }
    fprintf(fd, "\n");})

DEF_CMD(CALL, 21, "call", CallHandling(i, labels), CallHandling(int& i, std::unordered_map<int, int>& labels),
    {comm_buff.push_back(CMD_CALL);
    std::string lab = {};

    if (ReadLabel(i, commands, commands_len, lab)) {
        PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ReadLabel\n")
    }

    if (labels.find(lab) == labels.end()) {
        labels.insert({lab, i});
        comm_buff.push_back(i);
    } else {
        comm_buff.push_back(labels[lab]);
    }},

    {CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command jump (no argument)\n")
    fprintf(fd, "call %d\n", comm_buff[++i]);})
