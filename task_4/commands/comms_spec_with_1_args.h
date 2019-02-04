DEF_CMD(LABEL, 13, "label", , ,
    {comm_buff.push_back(CMD_LABEL);
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

    {CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command label (no argument)\n")
    fprintf(fd, "label %d\n", comm_buff[++i]);})
