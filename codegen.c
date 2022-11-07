
#include <stddef.h>
#include <stdio.h>

#include "common.h"
#include "parse.h"
#include "codegen.h"

void genAsm(void)
{
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", LValDict.ValsCount * 8);

    for (int i = 0; i < StatementsCount; i++)
    {
        genAsmSingleStatement(ProgramBuf[i]);
        printf("  pop rax\n");
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");

    printf("  ret\n");
}

void genAddr(Node* node)
{
    if (node->Type != NT_LVAL)
    {
        error("left side of assign must have an address");
    }

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->LValOffset);
    printf("  push rax\n");
}

void genAsmSingleStatement(Node* node)
{
    if (node->Type == NT_LVAL)
    {
        genAddr(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    }
    if (node->Type == NT_NUM)
    {
        printf("  push %d\n", node->Value);
        return;
    }
    if (node->Type == NT_ASSIGN)
    {
        genAddr(node->Lhs);
        genAsmSingleStatement(node->Rhs);
        printf("  pop rax\n");
        printf("  pop rdi\n");
        printf("  mov [rdi], rax\n");
        printf("  push rax\n");
        return;
    }

    genAsmSingleStatement(node->Lhs);
    genAsmSingleStatement(node->Rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch(node->Type)
    {
        case NT_ADD:
            printf("  add rax, rdi\n");
            break;
        case NT_SUB:
            printf("  sub rax, rdi\n");
            break;
        case NT_MUL:
            printf("  imul rax, rdi\n");
            break;
        case NT_DIV:
            printf("  cqo\n");
            printf("  idiv rax, rdi\n");
            break;
        case NT_EQUAL:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case NT_NEQUAL:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case NT_GREATER:
            printf("  cmp rdi, rax\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case NT_GREATEREQUAL:
            printf("  cmp rdi, rax\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        case NT_LESS:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case NT_LESSEQUAL:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
    }

    printf("  push rax\n");
}


