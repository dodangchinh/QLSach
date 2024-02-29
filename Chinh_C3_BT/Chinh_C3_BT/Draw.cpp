#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "math.h"
#include "windows.h"
#include "time.h"
#include "string.h"
#include "CType.h"

const int MAXCAT = 10; // danh mục tối đa
const int MAXBOOK = 100; // bài hát tối đa
const int MAXLINE = 256; // mỗi dòng tối đa 256 kí tự
const int MAX = 10;

struct Date
{
	int day;
	int month;
	int year;
};

struct Category
{
	char *id = new char[MAXLINE];
	char *name = new char[MAXLINE];
	int status = 1;

	Category *pNext;
	Category *pPrev;
};

struct ListCategory
{
	int n = 0;

	Category *pHead; // con trỏ nút đầu
	Category *pTail; // con trỏ nút cuối
};

struct BOOK
{
	char *isbn = new char[MAXLINE];
	char *name = new char[256];
	char *author = new char[256];
	int price;
	Category category; // loading related 
	char *publisher = new char[256];
	Date publishDate;
	int status = 1;

	BOOK *pNext;
	BOOK *pPrev;
};

struct ListBook
{
	int n = 0;

	BOOK *pHead; // con trỏ nút đầu
	BOOK *pTail; // con trỏ nút cuối
};

struct Database
{
	char **database;
	int iLines = 0;
};

void DrawRow(int n, int m, int Number1, int Number2, int Number3, int Number4, int flag)
{
	int temp = n;

	if (flag == 1)
	{
		printf("\n");
		Draw(Number1);
		for (int i = 0; i < 6; i++)
			Draw(Number3);
		Draw(Number2);
		temp -= 21;
	}
	n = temp;
	temp -= m;
	for (int idx = n; idx > 0; idx--)
	{
		if (idx == temp)
		{
			Draw(Number2);
			temp -= m;
		}
		else
			Draw(Number3);
	}
	Draw(Number4);
	printf("\n");
}

void DrawSpace(int n)
{
	while (n > 0)
	{
		printf(" ");
		n--;
	}
}

void DrawTitle(char c[], int n, int m, int count)
{
	Draw(186);
	DrawSpace(n);
	if (count == 0)
		printf("%s", c);
	else
		printf("%s %d", c, count);
	DrawSpace(m);
}

void DrawRow(char c[], int n, int m)
{
	int l = strlen(c) + n + m;
	for (int i = 0; i < l; i++)
		Draw(205);
}


void DrawTitle(int length, int flag)
{
	if (flag == 1)
	{
		DrawTitle("ID", 2, 2, 0);
		DrawTitle("Name", 5, 5, 0);
		DrawTitle("Author", 5, 6, 0);
		DrawTitle("Price", 2, 3, 0);
		DrawTitle("Category", 1, 1, 0);
		DrawTitle("Publisher", 4, 4, 0);
		DrawTitle("Date Load", 5, 6, 0);
	}
	else
	{
		DrawTitle("ID", 2, 2, 0);
		DrawTitle("Name Genres", 4, 6, 0);
	}

	DrawTitle("Status", 6, 8, 0);
	Draw(186);
}

void OutputTitle(int Value, int flag, int flags)
{
	int lengthBody = Value * 21;
	if (flag == 1)
	{
		Draw(201);
		DrawRow("ID", 2, 2);
		Draw(203);
		DrawRow("Name", 5, 5);
		Draw(203);
		DrawRow("Author", 5, 6);
		Draw(203);
		DrawRow("Price", 2, 3);
		Draw(203);
		DrawRow("Category", 1, 1);
		Draw(203);
		DrawRow("Publisher", 4, 4);
		Draw(203);
		DrawRow("Date Load", 5, 6);
		Draw(203);
		DrawRow("Status", 6, 8);
		Draw(187);
		printf("\n");
	}
	else
		DrawRow(lengthBody, 21, 201, 203, 205, 187, 1);

	DrawTitle(lengthBody / 21, flag);

}

void DrawSTT()
{
	printf("\n");
	Draw(204);
	for (int i = 0; i < 6; i++)
		Draw(205);
	Draw(206);
}

void DrawRow(int n, int a, int m)
{
	printf("\n");
	Draw(n);
	DrawRow("ID", 2, 2);
	Draw(a);
	DrawRow("Name", 5, 5);
	Draw(a);
	DrawRow("Author", 5, 6);
	Draw(a);
	DrawRow("Price", 2, 3);
	Draw(a);
	DrawRow("Category", 1, 1);
	Draw(a);
	DrawRow("Publisher", 4, 4);
	Draw(a);
	DrawRow("Date Load", 5, 6);
	Draw(a);
	DrawRow("Status", 6, 8);
	Draw(m);
	printf("\n");
}

void Draw(int n, char c)
{
	for (int i = 1; i <= n; i++)
		printf("%c", c);
}

void Draw(int spaceV1, int spaceV2, int count, char c)
{
	for (int i = 1; i <= spaceV2 - (count + spaceV1); i++)
		printf("%c", c);
}

void DrawChar(char *Index, int spaceV1, int spaceV2, int flag)
{
	if (flag == 1)
		Draw(186);
	Draw(spaceV1, ' ');
	printf("%s", Index);
	Draw(spaceV1, spaceV2, 1, ' ');
}

void DrawInt(int Index, int spaceV1, int spaceV2, int flag)
{
	if (flag == 1)
		Draw(186);
	Draw(spaceV1, ' ');
	printf("%d", Index);
	Draw(spaceV1, spaceV2, 1, ' ');
}

int CountIndex(int n)
{
	int count = 0;
	if (n == 0)
		return 1;

	while (n > 0)
	{
		n /= 10;
		count++;
	}
	return count;
}

void DrawInt(int Value, int index, int space)
{
	int lengthChar = index - CountIndex(Value);
	DrawInt(Value, space, lengthChar, 0);
	Draw(186);
}

void DrawName(char *Char, int index, int space)
{
	int lengthChar = index - getLength(Char);
	DrawChar(Char, space, lengthChar, 0);
	Draw(186);
}

void DrawStatus(int Value, int values)
{
	char *string;
	if (Value == 1)
	{
		SET_COLOR(9);
		string = "Active";
	}

	else
	{
		SET_COLOR(12);
		string = "Lock";
	}

	int lengthChar = 21 - getLength(string);
	DrawChar(string, 6, lengthChar, 0);
	SET_COLOR(15);
	Draw(186);
}

void Output(ListCategory lstCategory)
{
	OutputTitle(3, 0, 0);

	Category *temp = lstCategory.pHead;
	while (temp != NULL)
	{
		DrawSTT();
		DrawRow(42, 21, 204, 206, 205, 185, 0);
		Draw(186);
		DrawName(temp->id, 7, 2);
		DrawName(temp->name, 22, 4);
		DrawStatus(temp->status, 1);
		temp = temp->pNext;
	}
	DrawRow(63, 21, 200, 202, 205, 188, 1);
}

void Travesing(ListBook lstBook)
{
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		printf("%s, %s, %s, %d,", temp->isbn, temp->name, temp->author, temp->price);
		printf(" %s, %s, %d\%d\%d, %d\n", temp->category.name, temp->publisher, temp->publishDate.day, temp->publishDate.month, temp->publishDate.year, temp->status);
		temp = temp->pNext;
	}
}

void DrawDate(Date date, int spaceV1, int spaceV2)
{
	Draw(spaceV1, ' ');
	if (date.day < 10)
		printf("0%d/", date.day);
	else
		printf("%d/", date.day);

	if (date.month < 10)
		printf("0%d/", date.month);
	else
		printf("%d/", date.month);
	printf("%d", date.year);
	Draw(spaceV1, spaceV2, 11, ' ');
}

void DrawNameDate(Date date)
{
	DrawDate(date, 4, 21);
	Draw(186);
}

void Output(ListBook lstBook)
{
	system("cls");
	OutputTitle(5, 1, 1);
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		DrawRow(204, 206, 185);
		Draw(186);
		DrawName(temp->isbn, 7, 1);
		DrawName(temp->name, 15, 1);
		DrawName(temp->author, 18, 1);
		DrawInt(temp->price, 11, 2);
		DrawName(temp->category.id, 11, 4);
		DrawName(temp->publisher, 18, 3);
		DrawNameDate(temp->publishDate);
		DrawStatus(temp->status, 1);
		temp = temp->pNext;
	}
	DrawRow(200, 202, 188);
}