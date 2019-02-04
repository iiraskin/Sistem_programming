DEF_CMD(ADD, 0, "add", AddHandling(), AddHandling(),
    {comm_buff.push_back(CMD_ADD);},
    {fprintf(fd, "add\n");})

DEF_CMD(SUB, 1, "sub", SubHandling(), SubHandling(),
    {comm_buff.push_back(CMD_SUB);},
    {fprintf(fd, "sub\n");})

DEF_CMD(MUL, 2, "mul", MulHandling(), MulHandling(),
    {comm_buff.push_back(CMD_MUL);},
    {fprintf(fd, "mul\n");})

DEF_CMD(DIV, 3, "div", DivHandling(),  DivHandling(),
    {comm_buff.push_back(CMD_DIV);},
    {fprintf(fd, "div\n");})

DEF_CMD(SQR, 4, "sqr", SqrHandling(), SqrHandling(),
    {comm_buff.push_back(CMD_SQR);},
    {fprintf(fd, "sqr\n");})

DEF_CMD(IN, 5, "in", InHandling(), InHandling(),
    {comm_buff.push_back(CMD_IN);},
    {fprintf(fd, "in\n");})

DEF_CMD(OUT, 6, "out", OutHandling(), OutHandling(),
    {comm_buff.push_back(CMD_OUT);},
    {fprintf(fd, "out\n");})

DEF_CMD(RET, 22, "ret", RetHandling(i), RetHandling(int& i),
    {comm_buff.push_back(CMD_RET);},
    {fprintf(fd, "ret\n");})

DEF_CMD(MEOW, 25, "meow", MeowHandling(), MeowHandling(),
    {comm_buff.push_back(CMD_MEOW);},
    {fprintf(fd, "meow\n");})
