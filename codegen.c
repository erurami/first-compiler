
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"
#include "parse.h"
#include "codegen.h"


void genAsmRecursive(Node* node);
void genAddr(Node* node);
void genFuncParamPassingAsm(Node* node, int paramCount);
void genAsmFunctionArg(Node* node);



void genAsm(Program* program)
{
    ProgramElem* cur;

    if (program == NULL) return;
    printf(".intel_syntax noprefix\n");
    printf(".global ");

    cur = program->FirstElem;
    while (cur != NULL)
    {
        if (cur != program->FirstElem)
        {
            printf(", ");
        }
        printf("%.*s", cur->Len, cur->Str);
        cur = cur->Next;
    }

    printf("\n");

    cur = program->FirstElem;
    while (cur != NULL)
    {
        genAsmRecursive(program->FirstElem->Node);
        cur = cur->Next;
    }
}

// TODO: Separate function
//        * by the node has value or not
void genAsmRecursive(Node* node)
{
    if (node == NULL) return;

    switch (node->Type)
    {
        case NT_FUNCTION_DEF:
            printf("%.*s:\n", node->Len, node->Str);
            printf("  push rbp\n");
            printf("  mov rbp, rsp\n");
            printf("  sub rsp, %d\n", node->Value * 8);

            genAsmFunctionArg(node->Lhs);
            genAsmRecursive(node->Rhs);
            return;

        case NT_LVAL:
            genAddr(node);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;

        case NT_NUM:
            printf("  push %d\n", node->Value);
            return;

        case NT_ASSIGN:
            genAddr(node->Lhs);
            genAsmRecursive(node->Rhs);
            printf("  pop rax\n");
            printf("  pop rdi\n");
            printf("  mov [rdi], rax\n");
            printf("  push rax\n");
            return;

        case NT_STATEMENT:
            genAsmRecursive(node->Lhs);
            if (node->Lhs->HasValue) printf("  pop rax\n");
            genAsmRecursive(node->Rhs);
            if (node->Rhs->HasValue) printf("  pop rax\n");
            return;

        case NT_RETURN:
            genAsmRecursive(node->Lhs);
            printf("  pop rax\n");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            return;

        case NT_IF:
            genAsmRecursive(node->Lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lelse%03d\n", node->Value);
            genAsmRecursive(node->Rhs->Lhs);
            printf("  jmp .Lend%03d\n", node->Value);
            printf(".Lelse%03d:\n", node->Value);
            genAsmRecursive(node->Rhs->Rhs);
            printf(".Lend%03d:\n", node->Value);
            return;

        case NT_WHILE:
            printf(".Lwhilestart%03d:\n", node->Value);
            genAsmRecursive(node->Lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lwhileend%03d\n", node->Value);
            genAsmRecursive(node->Rhs);
            printf("  jmp .Lwhilestart%03d\n", node->Value);
            printf(".Lwhileend%03d:\n", node->Value);
            return;

        case NT_FUNCTION_CALL:
            if (node->Value > 6)
            {
                error("function with more the 6 parameters is not suppoted.");
            }
            genFuncParamPassingAsm(node->Rhs, node->Value);
            printf("  mov rax, %d\n", node->Value);
            printf("  call %.*s\n", node->Len, node->Str);
            printf("  push rax\n");
            return;

        case NT_DEREFERENCE:
            genAsmRecursive(node->Lhs);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;

        case NT_ADDRESS:
            genAddr(node->Lhs);
            return;
    }

    genAsmRecursive(node->Lhs);
    genAsmRecursive(node->Rhs);

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


void genFuncParamPassingAsm(Node* node, int paramCount)
{
    for (int i = 0 ; i < paramCount; i++)
    {
        genAsmRecursive(node->Lhs);
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

void genAsmFunctionArg(Node* node)
{
    if (node == NULL) return;
    Node* argument_node;
    for (int i = 0; i < 6; i++)
    {
        argument_node = node;
        if (node->Type == NT_FUNCTION_ARGUMENT) argument_node = node->Lhs;

        genAddr(argument_node);
        printf("  pop rax\n");
        switch (i)
        {
            case 0:
                printf("  mov [rax], rdi\n");
                break;
            case 1:
                printf("  mov [rax], rsi\n");
                break;
            case 2:
                printf("  mov [rax], rdx\n");
                break;
            case 3:
                printf("  mov [rax], rcx\n");
                break;
            case 4:
                printf("  mov [rax], r8\n");
                break;
            case 5:
                printf("  mov [rax], r9\n");
                break;
        }

        if (node->Type != NT_FUNCTION_ARGUMENT) return;
        node = node->Rhs;
    }
    error("cannnot declare function with more than 6 arguments");
    return;
}

void genAddr(Node* node)
{
    if (node->IsOnRam == false)
    {
        error("left side of assign must have an address");
    }

    if (node->Type == NT_DEREFERENCE)
    {
        genAsmRecursive(node->Lhs);
        return;
    }

    if (node->Type == NT_LVAL)
    {
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", node->Value * 8);
        printf("  push rax\n");
        return;
    }
}
