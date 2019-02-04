DEF_CMD(CMP, 12, "cmp", CmpHandling(i), CmpHandling(int& i),
    {comm_buff.push_back(CMD_CMP);

    if (ParseRegister(i, commands, commands_len)) {
        PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ParseRegister\n")
    }
    if (ParseRegister(i, commands, commands_len)) {
        PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ParseRegister\n")
    }},

    {fprintf(fd, "cmp ");
    for (int j = 0; j < 2; ++j) {
        CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command pop (no argument)\n")

        if (PrintRegister(fd, i)) {
            PARSER_ERROR("CRPNHandler::PrintWords: Error in CRPNHandler::PrintRegister\n")
        }
    }
    fprintf(fd, "\n");})
