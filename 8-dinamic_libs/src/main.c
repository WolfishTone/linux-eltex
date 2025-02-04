#include <./header.h>
#include <stdio.h>

enum
{
  ADD = '1',
  SUB,
  MUL,
  DIV,
  EXIT,
  SCANF_ERROR
};

void
print_actions ()
{
  printf (" Выберите пункт меню и введите его номер\n1) Сложение\n2) "
          "Вычитание\n3) Умножение\n4) Деление\n5) Выход\n <+>> ");
}

char
read_action ()
{
  char action;
  if (!scanf (" %c", &action))
    return SCANF_ERROR;
  while (action < ADD || action > EXIT)
    {
      printf ("недопустимый символ\n <+>> ");
      if (!scanf (" %c", &action))
        return SCANF_ERROR;
    }
  return action;
}

int
read_op (int *op)
{
  if (scanf ("%d", op))
    return 0;
  return 1;
}

int
main ()
{
  _Bool is_running = 1;
  while (is_running)
    {
      print_actions ();
      char action = read_action ();

      int op1, op2;
      int read_op_result = 0;

      switch (action)
        {
        case ADD:
          printf ("введите операнды <+>> ");
          read_op_result += read_op (&op1);
          read_op_result += read_op (&op2);
          printf ("read_op_result= %d\n", read_op_result);
          if (read_op_result)
            printf ("ошибка считывания операндов\n");
          else
            printf ("   %d + %d = %d\n", op1, op2, add (op1, op2));
          break;
        case SUB:
          printf ("введите операнды <+>> ");
          read_op_result += read_op (&op1);
          read_op_result += read_op (&op2);
          if (read_op_result)
            printf ("ошибка считывания операндов\n");
          else
            printf ("   %d - %d = %d\n", op1, op2, sub (op1, op2));
          break;
        case MUL:
          printf ("введите операнды <+>> ");
          read_op_result += read_op (&op1);
          read_op_result += read_op (&op2);
          if (read_op_result)
            printf ("ошибка считывания операндов\n");
          else
            printf ("   %d * %d = %d\n", op1, op2, mul (op1, op2));
          break;
        case DIV:
          printf ("введите операнды <+>> ");
          read_op_result += read_op (&op1);
          read_op_result += read_op (&op2);
          if (read_op_result)
            printf ("ошибка считывания операндов\n");
          else
            printf ("   %d / %d = %d\n", op1, op2, div (op1, op2));
          break;
        case EXIT:
          is_running = 0;
          break;
        case SCANF_ERROR:
          printf ("ошибка считывания\n");
        }
    }
  return 0;
}
