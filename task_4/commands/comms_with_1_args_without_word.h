DEF_CMD(PUSHR, 8, "push", PushRHandling(i), PushRHandling(int &i), ,
    {fprintf(fd, "push ");
    CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command push (no argument)\n")

    if (PrintRegister(fd, i)) {
        PARSER_ERROR("CRPNHandler::PrintWords: Error in CRPNHandler::PrintRegister\n")
    }
    fprintf(fd, "\n");})

DEF_CMD(PUSHRAM, 10, "push", PushRAMHandling(i), PushRAMHandling(int &i), ,
    {CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command push (no argument)\n")
    fprintf(fd, "push [%d]\n", comm_buff[++i]);})

DEF_CMD(POPRAM, 11, "pop", PopRAMHandling(i), PopRAMHandling(int &i), ,
    {CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command push (no argument)\n")
    fprintf(fd, "pop [%d]\n", comm_buff[++i]);})
