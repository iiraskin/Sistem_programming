DEF_CMD(BEGIN, 23, "begin", , ,
    {comm_buff.push_back(CMD_BEGIN);},
    {fprintf(fd, "begin\n");})

DEF_CMD(END, 24, "end", , ,
    {comm_buff.push_back(CMD_END);},
    {fprintf(fd, "end\n");})
