
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"
#include "parse.h"
#include "codegen.h"

bool IsThereReturn;

void genAsmSingleStatement(Node* node);

void genAsm(Node* node)
{
    IsThereReturn = false;

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", LValDict.ValsCount * 8);

    genAsmSingleStatement(node);

    if (IsThereReturn == false)
    {
        error("no return statement.");
    }
}

void genFuncParamPassingAsm(Node* node, int paramCount)
{
    for (int i = 0 ; i < paramCount; i++)
    {
        genAsmSingleStatement(node->Lhs);
        node = node->Rhs;
    }

    for (int i = paramCount; i > 0; i--)
    {
        switch (i)
        {
            case 1:
                printf("  pop rdi\n");
                break;
            case 2:
                printf("  pop rsi\n");
                break;
            case 3:
                printf("  pop rdx\n");
                break;
            case 4:
                printf("  pop rcx\n");
                break;
            case 5:
                printf("  pop r8\n");
                break;
            case 6:
                printf("  pop r9\n");
                break;
        }
    }
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
    if (node == NULL) return;
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
    if (node->Type == NT_STATEMENT)
    {
        genAsmSingleStatement(node->Lhs);
        if (node->Lhs->HasValue) printf("  pop rax\n");
        genAsmSingleStatement(node->Rhs);
        if (node->Rhs->HasValue) printf("  pop rax\n");
        return;
    }
    if (node->Type == NT_RETURN)
    {
        genAsmSingleStatement(node->Lhs);
        printf("  pop rax\n");

        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");

        printf("  ret\n");
        IsThereReturn = true;
        return;
    }
    if (node->Type == NT_IF)
    {
        genAsmSingleStatement(node->Lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%03d\n", node->IfId);
        genAsmSingleStatement(node->Rhs->Lhs);
        printf("  jmp .Lend%03d\n", node->IfId);
        printf(".Lelse%03d:\n", node->IfId);
        genAsmSingleStatement(node->Rhs->Rhs);
        printf(".Lend%03d:\n", node->IfId);
        return;
    }
    if (node->Type == NT_WHILE)
    {
        printf(".Lwhilestart%03d:\n", node->WhileId);
        genAsmSingleStatement(node->Lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lwhileend%03d\n", node->WhileId);
        genAsmSingleStatement(node->Rhs);
        printf("  jmp .Lwhilestart%03d\n", node->WhileId);
        printf(".Lwhileend%03d:\n", node->WhileId);
        return;
    }
    if (node->Type == NT_FUNCTION)
    {
        if (node->FuncParamCount > 6)
        {
            error("function with more the 6 parameters is not suppoted.");
        }
        genFuncParamPassingAsm(node->Rhs, node->FuncParamCount);
        printf("  mov rax, %d\n", node->FuncParamCount);
        printf("  call %.*s\n", node->FuncNameLen, node->pFuncName);
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


