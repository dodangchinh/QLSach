// Chinh_C3_BT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "math.h"
#include "windows.h"
#include "time.h"
#include "string.h"
#include "CType.h"
#include "Draw.h";

const int MAXCAT = 10; // danh mục tối đa
const int MAXBOOK = 100; // bài hát tối đa
const int MAXLINE = 256; // mỗi dòng tối đa 256 kí tự
const int MAX = 10;

char *fileNameCate = "data/Categories.txt";
char *fileNameBook = "data/Books.txt";
char *mode = "rt";

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

void(*adminPointer)(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook);

void Notify(int flag)
{
	switch (flag)
	{
	case 1:
		printf("Do you want to continue? Press (1)Y or (ESC)N!\n");
		break;
	case 2:
		printf("This name already exists!\n");
		break;
	case 3:
		printf("This name does not exist!\n");
		break;
	case 4:
		printf("Successfully!\n");
		break;
	case 5:
		printf("Not found!\n");
		break;
	case 6:
		printf("Press (1)Y or (ESC)N!\n");
		break;
	case 7:
		printf("This genre has been locked!\n");
		break;
	case 8:
		printf("There are books in the same genre. Please enter again the category!\n");
		break;
	case 9:
		printf("This book's category has been locked!\n");
		break;
	case 10:
		printf("Successfully! Pess (ESC) to exits!");
		break;
	}
}

void  OutputTitle(char c[])
{
	system("cls");
	printf("\t\t\t%c%s%c", 175, c, 174);
	printf_s("\n");
}

int ExitAdmin(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char ch = _getch();
	if (ch == 27)
		adminPointer(f, db, lstCategory, lstBook);
	if (ch == 49)
		return 0;
}

void OpenFile(FILE *&f, char *fileName, char *mode)
{
	errno_t err = fopen_s(&f, fileName, mode);
	if (err != 0)
		printf("The file was not opened\n");
}

void Input(FILE *&f, Database &db, char *fileName, char *mode)
{
	OpenFile(f, fileName, mode);
	fscanf_s(f, "%d", &db.iLines);
	fgetc(f); // pass qua 1 kí tự \n
	db.database = new char*[db.iLines];

	int index = 0;
	while (!feof(f))
	{
		db.database[index] = new char[MAXLINE];
		fgets(db.database[index], MAXLINE, f);
		index++;
	}
	fclose(f);
}

int ChuyenDoiSo(char c)
{
	if (c >= 48 && c <= 57)
		return c - 48;
	return -1; // không chuyển đổi được
}

int ChuyenDoiChuoiSo(char *s)
{
	int length = strlen(s);
	int number = 0;
	for (int i = 0; i < strlen(s); i++)
	{
		number += ChuyenDoiSo(s[i]) * pow(10 * 1.0, (length - 1) * 1.0);
		length--;
	}
	return number;
}

char *TachTheoKiTu(char *str, char delim, char *&next)
{
	if (str != NULL)
		next = str;
	str = next;
	for (int i = 0; i < strlen(str); i++)
	{
		next++;
		if (str[i] == delim || i == strlen(str) - 1)
			str[i] = '\0';
	}

	if (strlen(str) == 0)
		return NULL;
	return str;
}

int isEmpty(ListCategory lstCate)
{
	if ((lstCate.pHead == NULL) && (lstCate.pTail == NULL))
		return 1; // là danh sách rỗng
	return 0;
}

int isEmpty(ListBook lstBook)
{
	if ((lstBook.pHead == NULL) && (lstBook.pTail == NULL))
		return 1; // là danh sách rỗng
	return 0;
}

Category *createCate(char *&id, char *&name, int &status)
{
	Category *p = new Category;
	if (p == NULL)
		return NULL;

	p->id = id;
	p->name = name;
	p->status = status;
	p->pNext = NULL;
	p->pPrev = NULL;
	return p;
};

BOOK *createBook(char *isbn, char *name, char *author, int price, Category category, char *publisher, Date publishDate, int &status)
{
	BOOK *p = new BOOK;
	if (p == NULL)
		return NULL;

	p->isbn = isbn;
	p->name = name;
	p->author = author;
	p->price = price;
	p->category = category;
	p->publisher = publisher;
	p->publishDate = publishDate;
	p->status = status;

	p->pNext = NULL;
	p->pPrev = NULL;
	return p;
};

void addTail(ListCategory &lstCategory, Category *newCate)
{
	if (isEmpty(lstCategory))
		lstCategory.pHead = lstCategory.pTail = newCate;
	else {
		lstCategory.pTail->pNext = newCate;
		newCate->pPrev = lstCategory.pTail;
		lstCategory.pTail = newCate;
	}
}

void LoadCate(ListCategory &lstCategory, Database db)
{
	Category *newCate;
	char *temp;
	char delim = ',';
	char *next;
	char *id;
	char*name;
	int status;
	lstCategory.n = db.iLines;

	for (int i = 0; i < db.iLines; i++)
	{
		temp = TachTheoKiTu(db.database[i], delim, next);
		id = temp;

		while (temp != NULL)
		{
			temp = TachTheoKiTu(NULL, delim, next);
			name = temp;

			temp = TachTheoKiTu(NULL, delim, next);
			status = ChuyenDoiChuoiSo(temp);
			if (*next == '\n' || *next == '\0')
				break;
		}
		newCate = createCate(id, name, status);
		addTail(lstCategory, newCate);
	}
}

Category *getCategory(ListCategory lstCategory, char *isbn)
{
	Category *p = lstCategory.pHead;
	for (int i = 0; i < lstCategory.n; i++)
	{
		if (Compare(p->id, isbn) == 0)
			return p;
		p = p->pNext;
	}
	return NULL;
}

void addTail(ListBook &lstBook, BOOK *newBook)
{
	if (isEmpty(lstBook))
		lstBook.pHead = lstBook.pTail = newBook;
	else {
		lstBook.pTail->pNext = newBook;
		newBook->pPrev = lstBook.pTail;
		lstBook.pTail = newBook;
	}
}

void LoadBook(ListBook &lstBook, ListCategory &lstCategory, Database db)
{
	BOOK *newBook;
	char *temp;
	char delim = ',';
	char *next;

	char *isbn = new char[MAXLINE];
	char *name = new char[256];
	char *author = new char[256];
	int price;
	Category category; // loading related 
	char *publisher = new char[256];
	Date publishDate;
	int status = 1;

	lstBook.n = db.iLines;

	for (int i = 0; i < db.iLines; i++)
	{
		// get id
		temp = TachTheoKiTu(db.database[i], delim, next);
		isbn = temp;
		while (temp != NULL)
		{
			temp = TachTheoKiTu(NULL, delim, next);
			name = temp;
			temp = TachTheoKiTu(NULL, delim, next);
			author = temp;
			temp = TachTheoKiTu(NULL, delim, next);
			price = ChuyenDoiChuoiSo(temp);
			temp = TachTheoKiTu(NULL, delim, next);
			category = *getCategory(lstCategory, temp);
			temp = TachTheoKiTu(NULL, delim, next);
			publisher = temp;

			// get day, month, year
			temp = TachTheoKiTu(NULL, delim, next);
			publishDate.day = ChuyenDoiChuoiSo(temp);
			temp = TachTheoKiTu(NULL, delim, next);
			publishDate.month = ChuyenDoiChuoiSo(temp);
			temp = TachTheoKiTu(NULL, delim, next);
			publishDate.year = ChuyenDoiChuoiSo(temp);
			temp = TachTheoKiTu(NULL, delim, next);

			status = ChuyenDoiChuoiSo(temp);

			if (*next == '\n' || *next == '\0')
				break;
		}
		newBook = createBook(isbn, name, author, price, category, publisher, publishDate, status);
		addTail(lstBook, newBook);
	}
}

void Init(ListCategory &lstCategory)
{
	lstCategory.pHead = lstCategory.pTail = NULL;
}

void Init(ListBook &lstBooks)
{
	lstBooks.pHead = lstBooks.pTail = NULL;
}

void addHead(ListBook &lstBook, BOOK *newNode)
{
	if (isEmpty(lstBook))
		lstBook.pHead = lstBook.pTail = newNode;
	else
	{
		newNode->pNext = lstBook.pHead;
		lstBook.pHead->pPrev = newNode;
		lstBook.pHead = newNode;
	}
}

void addAfter(ListBook lstBook, BOOK *p, BOOK *newNode)
{
	if (p != NULL) {
		newNode->pNext = p->pNext;
		p->pNext->pPrev = newNode;
		newNode->pPrev = p;
		p->pNext = newNode;

		if (p == lstBook.pTail)
			lstBook.pTail = newNode;
	}
	else
		addHead(lstBook, newNode);
}

int isEmpty(char *String)
{
	if (String[0] == '\0')
		return 0;
	return 1;
}

int StartTrim(char *&source)
{
	int n = getLength(source);
	for (int idx = 0; idx < n; idx++)
	{
		if (source[idx] != ' ')
			return idx;
	}
}

int EndTrim(char *&source)
{
	int n = getLength(source);
	for (int idx = n - 1; idx >= 0; idx--)
	{
		if (source[idx] != ' ')
			return idx;
	}
}

void Trim(char *&source)
{
	char *dest = new char[50];
	int start = StartTrim(source);
	int end = EndTrim(source);
	int i = 0;

	for (int idx = start; idx <= end; idx++)
	{
		if (source[idx] == ' ' && source[idx + 1] == ' ')
			continue;
		else
			dest[i] = source[idx];
		i++;
	}
	dest[i] = '\0';
	source = dest;
}


void ShowView(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	int flag;
	char c = 49;

	do
	{
		do
		{
			OutputTitle("LIST");
			printf("Selected View:\n");
			printf("1. Genres!\n");
			printf("2. Books!\n");
			printf("Options:");
			scanf_s("%d", &flag);
		} while (flag < 1 || flag > 2);

		if (flag == 1)
		{
			OutputTitle("LIST");
			Output(lstCategory);
		}
		if (flag == 2)
		{
			OutputTitle("LIST");
			Output(lstBook);
		}
		Notify(1);
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

int checkStatus(ListCategory lstCategory, ListBook lstBook, char *name, int flag)
{
	Category *temp = lstCategory.pHead;
	BOOK *temp2 = lstBook.pHead;
	switch (flag)
	{
	case 1:
		for (int i = 0; i < lstCategory.n; i++)
		{
			if (!_strcmpi(temp->id, name))
			{
				if (temp->status == 1)
					return 1;
				return 0;
			}
			temp = temp->pNext;
		}
		break;
	case 2:
		for (int i = 0; i < lstBook.n; i++)
		{
			if (!_strcmpi(temp2->author, name))
			{
				if (temp2->status == 1)
					return 1;
				return 0;
			}
			temp2 = temp2->pNext;
		}
	}
}

int isExistAuthor(char **author, int n, char *name)
{
	for (int i = 0; i < n; i++)
	{
		if (!_strcmpi(author[i], name))
			return 1;
	}
	return 0;
}

int isExist(ListCategory lstCategory, char *name)
{
	Category *temp = lstCategory.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->id, name))
			return 1;
		temp = temp->pNext;
	}
	return 0;
}

int isExistIsbn(ListBook lstBook, char *name)
{
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->isbn, name))
			return 1;
		temp = temp->pNext;
	}
	return 0;
}

int isExist(ListBook lstBook, char *name, int flag)
{
	BOOK *temp = lstBook.pHead;
	if (flag == 1)
	{
		while (temp != NULL)
		{
			if (!_strcmpi(temp->author, name))
				return 1;
			temp = temp->pNext;
		}
	}
	else
	{
		while (temp != NULL)
		{
			if (!_strcmpi(temp->name, name))
				return 1;
			temp = temp->pNext;
		}
	}
	return 0;
}

void saveAuthor(ListBook lstBook, char **&author, int &n, char *cate)
{
	BOOK *temp = lstBook.pHead;
	for (int i = 0; i < lstBook.n; i++)
	{
		if (!_strcmpi(temp->category.id, cate))
		{
			if (!isExistAuthor(author, n, temp->author))
			{
				author[n] = temp->author;
				n++;
			}
		}
		temp = temp->pNext;
	}
}

char *getAuthor(ListCategory lstCategory, ListBook lstBook, char **author, int n, char *cate)
{
	int max = 0;
	int pos;
	for (int i = 0; i < n; i++)
	{
		int nExist = 0;
		BOOK *temp = lstBook.pHead;
		for (int j = 0; j < lstBook.n; j++)
		{
			if (!_strcmpi(temp->category.id, cate))
			{
				if (!_strcmpi(author[i], temp->author))
					nExist++;
			}
			temp = temp->pNext;
		}
		if (nExist > max)
		{
			max = nExist;
			pos = i;
		}
	}
	return author[pos];
}

void OutputSearchCate(ListCategory lstCategory, char *cate)
{
	system("cls");
	OutputTitle(3, 0, 0);
	Category *temp = lstCategory.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->id, cate))
		{
			DrawSTT();
			DrawRow(42, 21, 204, 206, 205, 185, 0);
			Draw(186);
			DrawName(temp->id, 7, 2);
			DrawName(temp->name, 22, 4);
			DrawStatus(temp->status, 1);
		}
		temp = temp->pNext;
	}
	DrawRow(63, 21, 200, 202, 205, 188, 1);
}

void OutputSearchByCate(ListBook lstBook, char *cate, char *name)
{
	system("cls");
	OutputTitle(5, 1, 1);
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->category.id, cate))
		{
			if (!_strcmpi(temp->author, name))
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
			}
		}
		temp = temp->pNext;
	}
	DrawRow(200, 202, 188);
}

void OutputSearch(ListBook lstBook, char *name, int flag)
{
	system("cls");
	OutputTitle(5, 1, 1);
	BOOK *temp = lstBook.pHead;
	if (flag == 1)
	{
		while (temp != NULL)
		{
			if ((!_strcmpi(temp->author, name)) && temp->status == 1)
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
			}
			temp = temp->pNext;
		}
	}

	if (flag == 2)
	{
		while (temp != NULL)
		{
			if ((!_strcmpi(temp->author, name)))
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
			}
			temp = temp->pNext;
		}
	}
	DrawRow(200, 202, 188);
}

void OutputSearch(ListBook lstBook, int value)
{
	system("cls");
	OutputTitle(5, 1, 1);
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		if (temp->publishDate.year == value && temp->status == 1)
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
		}
		temp = temp->pNext;
	}
	DrawRow(200, 202, 188);
}

int getCharStatus(int status, char *&Char)
{
	Char = new char[10];
	if (status == 1)
	{
		Char = "Lock";
		return 1;
	}
	else
		Char = "Active";
	return 0;
}

int getStatus(ListBook lstBook, char *name)
{
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->author, name) && temp->category.status == 1)
			return temp->status;
		temp = temp->pNext;
	}
}

int isStatusBookCate(ListBook lstBook, char *name)
{
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->author, name))
			if (temp->category.status == 0)
				return 0;
		temp = temp->pNext;
	}
	return 1;
}

int checkStatus(ListBook lstBook, char *name, int status)
{
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->author, name))
			if (temp->status == status)
				return 1;
		temp = temp->pNext;
	}
	return 0;
}

int getStatus(ListCategory lstCategory, char *name)
{
	Category *temp = lstCategory.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->id, name))
			return temp->status;
		temp = temp->pNext;
	}
}

void AuthorStatus(FILE *f, ListBook lstBook, int status, char *author, char *mode, int flag)
{
	char *fileNameCate = "data/Books.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	fprintf_s(f, "%d", lstBook.n);
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->author, author) && flag == 1)
		{
			if (temp->status == status && status == 1)
				if (temp->status == 1 && temp->category.status == 1)
					temp->status = 0;
			if (temp->status == status && status == 0)
				if (temp->status == 0 && temp->category.status == 1)
					temp->status = 1;
		}
		fprintf_s(f, "\n%s,%s,%s,%d,", temp->isbn, temp->name, temp->author, temp->price);
		fprintf_s(f, "%s,%s,%d,%d,%d,%d,", temp->category.id, temp->publisher, temp->publishDate, temp->status);
		temp = temp->pNext;
	}
	fclose(f);
}

void CateStatus(FILE *f, ListCategory lstCategory, int status, char *cate, char *mode)
{
	char *fileNameCate = "data/Categories.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	fprintf_s(f, "%d", lstCategory.n);
	Category *temp = lstCategory.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->id, cate))
		{
			if (temp->status == status && status == 1)
				if (temp->status == 1)
					temp->status = 0;
			if (temp->status == status && status == 0)
				if (temp->status == 0)
					temp->status = 1;
		}
		fprintf_s(f, "\n%s,%s,%d,", temp->id, temp->name, temp->status);
		temp = temp->pNext;
	}
	fclose(f);
}

void CateStatus(FILE *f, ListBook lstBook, int status, char *author, char *mode)
{
	char *fileNameCate = "data/Books.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	fprintf_s(f, "%d", lstBook.n);
	BOOK *temp = lstBook.pHead;
	while (temp != NULL)
	{
		if (!_strcmpi(temp->category.id, author))
		{
			if (temp->status == status && status == 1)
				if (temp->status == 1)
					temp->status = 0;
			if (temp->status == status && status == 0)
				if (temp->status == 0)
					temp->status = 1;
		}
		fprintf_s(f, "\n%s,%s,%s,%d,", temp->isbn, temp->name, temp->author, temp->price);
		fprintf_s(f, "%s,%s,%d,%d,%d,%d,", temp->category.id, temp->publisher, temp->publishDate, temp->status);
		temp = temp->pNext;
	}
	fclose(f);
}

void LockActiveAuthor(FILE *f, ListCategory lstCategory, ListBook lstBook, char *author)
{
	char c;
	int flag = 1;
	char *Char;
	int status;

	int pos = 1;
	while (1)
	{
		pos = 1;
		while (pos)
		{
			OutputSearch(lstBook, author, 2);
			status = getStatus(lstBook, author);
			getCharStatus(status, Char);
			OutputSearch(lstBook, author, 2);
			printf("Do you wan to %s for %s? ", Char, author);
			Notify(6);

			c = _getch();
			if (c == 49)
			{
				AuthorStatus(f, lstBook, status, author, "wt", 1);
				OutputSearch(lstBook, author, 2);
				flag = 1;
			}
			else
				flag = 0;

			if (flag != -1 || flag == 0)
			{
				if (flag == 0)
					break;
				pos = 0;
			}
		}

		if (flag == 1)
		{
			Notify(4);
			break;
		}
		Notify(1);
		c = _getch();
		if (c == 27)
			break;
	}
}

void LockActiveCate(FILE *f, ListCategory lstCategory, ListBook lstBook, char *cate)
{
	char c;
	int flag = 1;
	char *Char;
	int status;

	int pos = 1;
	while (1)
	{
		pos = 1;
		while (pos)
		{
			OutputSearchCate(lstCategory, cate);
			status = getStatus(lstCategory, cate);
			getCharStatus(status, Char);
			OutputSearchCate(lstCategory, cate);
			printf("Do you wan to %s for %s? ", Char, cate);
			Notify(6);

			c = _getch();
			if (c == 49)
			{
				CateStatus(f, lstCategory, status, cate, "wt");
				if (status == 1)
					CateStatus(f, lstBook, status, cate, "wt");
				OutputSearchCate(lstCategory, cate);
				flag = 1;
			}
			else
				flag = 0;

			if (flag != -1 || flag == 0)
			{
				if (flag == 0)
					break;
				pos = 0;
			}
		}

		if (flag == 1)
		{
			Notify(4);
			break;
		}
		Notify(1);
		c = _getch();
		if (c == 27)
			break;
	}
}

int searchBook(FILE *f, ListCategory lstCategory, ListBook lstBook, char *name, int value, int flag)
{
	char c;
	int temp = 1;
	char **author = new char*[10];
	char *authorAppear;
	int n = 0;
	int max;

	if (temp != -1)
	{
		switch (flag)
		{
		case 1:
			saveAuthor(lstBook, author, n, name);
			authorAppear = getAuthor(lstCategory, lstBook, author, n, name);
			OutputSearchByCate(lstBook, name, authorAppear);
			break;
		case 2:
			OutputSearch(lstBook, name, 1);
			break;
		case 3:
			OutputSearch(lstBook, value);
			break;
		case 4:
			LockActiveAuthor(f, lstCategory, lstBook, name);
			break;
		case 5:
			LockActiveCate(f, lstCategory, lstBook, name);
			break;
		}
		temp = 0;
	}
	else
	{
		Notify(3);
		Notify(1);
		c = _getch();
		if (c == 27)
			temp = -1;
	}
	return temp;
}

void FeatureTwo(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char *findCate;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			findCate = new char[MAXLINE];
			OutputTitle("FEATURE TWO");
			Output(lstCategory);
			printf("Input ID genres: ");
			gets_s(findCate, MAXLINE);
			Trim(findCate);

			if (!isEmpty(findCate))
				continue;
			if (!checkStatus(lstCategory, lstBook, findCate, 1))
			{
				Notify(7);
				Notify(1);
				break;
			}
			if (!isExist(lstCategory, findCate))
			{
				Notify(5);
				Notify(1);
				break;
			}

			int temp = searchBook(f, lstCategory, lstBook, findCate, -1, 1);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

void FeatureThree(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char *findAuthor;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			findAuthor = new char[MAXLINE];
			OutputTitle("FEATURE THREE");
			printf("Input Author: ");
			gets_s(findAuthor, MAXLINE);
			Trim(findAuthor);

			if (!isEmpty(findAuthor))
				continue;
			if (!checkStatus(lstCategory, lstBook, findAuthor, 2))
			{
				Notify(7);
				Notify(1);
				break;
			}
			if (!isExist(lstBook, findAuthor, 1))
			{
				Notify(5);
				Notify(1);
				break;
			}

			int temp = searchBook(f, lstCategory, lstBook, findAuthor, -1, 2);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

int isExistYear(int *year, int n, int value)
{
	for (int i = 0; i < n; i++)
	{
		if (year[i] == value)
			return 1;
	}
	return 0;
}

void saveYear(ListBook lstBook, int *&year, int &n)
{
	BOOK *temp = lstBook.pHead;
	for (int i = 0; i < lstBook.n; i++)
	{
		if (!isExistYear(year, n, temp->publishDate.year))
		{
			year[n] = temp->publishDate.year;
			n++;
		}
		temp = temp->pNext;
	}
}

int getYear(ListBook lstBook, int *year, int n)
{
	int max = 0;
	int pos;
	for (int i = 0; i < n; i++)
	{
		int nExist = 0;
		BOOK *temp = lstBook.pHead;
		for (int j = 0; j < lstBook.n; j++)
		{
			if (year[i] == temp->publishDate.year)
				nExist++;
			temp = temp->pNext;
		}
		if (nExist > max)
		{
			max = nExist;
			pos = i;
		}
	}
	return year[pos];
}

void FeatureFour(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	int *year = new int[10];
	int yearAppear;
	int n = 0;
	int max;
	do
	{
		saveYear(lstBook, year, n);
		yearAppear = getYear(lstBook, year, n);
		searchBook(f, lstCategory, lstBook, NULL, yearAppear, 3);

		Notify(10);
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

BOOK *getPos(ListBook lstBook, int n)
{
	BOOK *temp = lstBook.pHead;
	while (n > 1)
	{
		temp = temp->pNext;
		n--;
	}
	return temp;
}

void Swap(BOOK *&tempH, BOOK *&tempT)
{
	int i;
	char *c;
	Category Cate;
	Date date;

	c = tempH->isbn;
	tempH->isbn = tempT->isbn;
	tempT->isbn = c;

	c = tempH->name;
	tempH->name = tempT->name;
	tempT->name = c;

	c = tempH->author;
	tempH->author = tempT->author;
	tempT->author = c;

	i = tempH->price;
	tempH->price = tempT->price;
	tempT->price = i;

	Cate = tempH->category;
	tempH->category = tempT->category;
	tempT->category = Cate;

	c = tempH->publisher;
	tempH->publisher = tempT->publisher;
	tempT->publisher = c;

	date = tempH->publishDate;
	tempH->publishDate = tempT->publishDate;
	tempT->publishDate = date;

	i = tempH->status;
	tempH->status = tempT->status;
	tempT->status = i;
}

int getPivot(ListBook lstBook, int l, int r)
{
	int n = (l + r) / 2;
	BOOK *temp = lstBook.pHead;
	while (n > 1)
	{
		temp = temp->pNext;
		n--;
	}
	return temp->price;
}

void QuickSort(ListBook lstBook, int l, int r)
{
	BOOK *tempH = getPos(lstBook, l);
	BOOK *tempT = getPos(lstBook, r);
	if (l >= r)
		return;

	int pivot = getPivot(lstBook, l, r);
	int i = l;
	int j = r;

	while (i <= j)
	{
		while (tempH->price < pivot)
		{
			tempH = tempH->pNext;
			i++;
		}

		while (tempT->price > pivot)
		{
			tempT = tempT->pPrev;
			j--;
		}

		if (i <= j)
		{
			Swap(tempH, tempT);
			tempH = tempH->pNext;
			i++;
			tempT = tempT->pPrev;
			j--;
		}
	}
	QuickSort(lstBook, l, j);
	QuickSort(lstBook, i, r);
}

void setData(ListBook lstBook, ListBook &tempB)
{
	BOOK *temp = lstBook.pHead;
	BOOK *newBook;

	char *isbn = new char[MAXLINE];
	char *name = new char[256];
	char *author = new char[256];
	int price;
	Category category; // loading related 
	char *publisher = new char[256];
	Date publishDate;
	int status;

	tempB.n = lstBook.n;

	while (temp != NULL)
	{
		isbn = temp->isbn;
		name = temp->name;
		author = temp->author;
		price = temp->price;
		category = temp->category;
		publisher = temp->publisher;
		publishDate = temp->publishDate;
		status = temp->status;

		newBook = createBook(isbn, name, author, price, category, publisher, publishDate, status);
		addTail(tempB, newBook);
		temp = temp->pNext;
	}
}

void FeatureFive(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	do
	{
		OutputTitle("FEATURE FIVE");
		ListBook tempB;
		Init(tempB);
		setData(lstBook, tempB);
		QuickSort(tempB, 1, lstBook.n);
		Output(tempB);
		Notify(10);
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

void ActiveLockAuthorView(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char *findAuthor;
	int status;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			findAuthor = new char[MAXLINE];
			OutputTitle("FEATURE SIX");
			printf("Input Author: ");
			gets_s(findAuthor, MAXLINE);
			Trim(findAuthor);

			if (!isEmpty(findAuthor))
				continue;

			if (!isExist(lstBook, findAuthor, 1))
			{
				Notify(5);
				Notify(1);
				break;
			}

			int temp = searchBook(f, lstCategory, lstBook, findAuthor, -1, 4);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

void ActiveLockCateView(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char *findCate;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			findCate = new char[MAXLINE];
			OutputTitle("FEATURE SIX");
			printf("Input Category: ");
			gets_s(findCate, MAXLINE);
			Trim(findCate);

			if (!isEmpty(findCate))
				continue;

			if (!isExist(lstCategory, findCate))
			{
				Notify(5);
				Notify(1);
				break;
			}

			int temp = searchBook(f, lstCategory, lstBook, findCate, -1, 5);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

void FeatureSix(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	do
	{
		int opt;
		do
		{
			OutputTitle("FEATURE SIX");
			printf("1. Active Lock Category!\n");
			printf("2. Active Lock Author!\n");

			printf("Options: ");
			opt = _getch();
		} while (opt < 49 || opt > 50);

		if (opt == 49)
			ActiveLockCateView(f, db, lstCategory, lstBook);
		if (opt == 50)
			ActiveLockAuthorView(f, db, lstCategory, lstBook);
		Notify(1);

	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

void AdminMenu(int &opt)
{
	do
	{
		OutputTitle("MENU");
		printf("1. Show List!\n");
		printf("2. Lists books by authors that appear most frequently in the category!\n");
		printf("3. Search for books by author!\n");
		printf("4. Find the year with the most books published!\n");
		printf("5. Arrange prices in ascending order!\n");
		printf("6. Lock or Active a book by author!\n");
		printf("7. Add books to position!\n");
		printf("Options: ");
		opt = _getch();
	} while (opt < 49 || opt > 55);
	system("cls");
}

char *SearchIsbn(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char *isbn;
	int temp;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			isbn = new char[MAXLINE];
			OutputTitle("FEATURE SIX");
			Output(lstBook);
			printf("Input ISBN To Insert: ");
			gets_s(isbn, MAXLINE);
			Trim(isbn);

			if (!isEmpty(isbn))
			{
				continue;
				temp = 0;
			}

			if (!isExistIsbn(lstBook, isbn))
			{
				Notify(5);
				Notify(1);
				temp = 0;
				break;
			}

			return isbn;

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

char *InputIsbn(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char *isbn;
	int temp;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			isbn = new char[MAXLINE];
			printf("Input ISBN: ");
			gets_s(isbn, MAXLINE);
			Trim(isbn);

			if (!isEmpty(isbn))
			{
				continue;
				temp = 0;
			}

			if (isExistIsbn(lstBook, isbn))
			{
				Notify(2);
				Notify(1);
				temp = 0;
				break;
			}

			return isbn;

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

char *InputName(char *c)
{
	char *name;
	int temp;
	int pos = 1;
	pos = 1;
	while (pos)
	{
		name = new char[MAXLINE];
		printf(c);
		gets_s(name, MAXLINE);
		Trim(name);

		if (!isEmpty(name))
		{
			continue;
			temp = 0;
		}

		return name;

		if (temp != -1 || temp == 0)
		{
			if (temp == 0)
				Notify(1);
			pos = 0;
		}
	}
}

int InputValue(char *c)
{
	int value;
	printf(c);
	scanf_s("%d", &value);
	return value;
}

Category *SelectedGenres(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char *findCate;
	int temp;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			findCate = new char[MAXLINE];
			Output(lstCategory);
			printf("Input Category ID: ");
			getchar();
			gets_s(findCate, MAXLINE);
			Trim(findCate);

			if (!isEmpty(findCate))
			{
				continue;
				temp = 0;
			}

			if (!isExist(lstCategory, findCate))
			{
				Notify(5);
				Notify(1);
				temp = 0;
				break;
			}

			if (!getStatus(lstCategory, findCate))
			{
				Notify(9);
				Notify(1);
				temp = 0;
				break;
			}

			Category *temp2 = lstCategory.pHead;
			while (temp2 != NULL)
			{
				if (!_strcmpi(temp2->id, findCate))
					return temp2;
				temp2 = temp2->pNext;
			}

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

Date InputDate()
{
	Date temp;
	int day;
	int month;
	int year;

	printf("Input date:\n");
	do
	{
		printf("Day: ");
		scanf_s("%d", &day);
	} while (day < 1 || day > 31);
	temp.day = day;

	do
	{
		printf("Month: ");
		scanf_s("%d", &month);
	} while (month < 1 || month > 12);
	temp.month = month;

	do
	{
		printf("Year: ");
		scanf_s("%d", &year);
	} while (year < 1000);
	temp.year = year;

	return temp;
}

BOOK *PosID(ListBook lstBook, char *isbn)
{
	BOOK *temp = lstBook.pHead;

	while (temp != NULL)
	{
		if (!_strcmpi(temp->isbn, isbn))
			return temp;
		temp = temp->pNext;
	}
}

int AddSong(FILE *f, Database db, ListCategory lstCategory, ListBook &lstBook)
{
	lstBook.n++;
	char *posIsbn = SearchIsbn(f, db, lstCategory, lstBook);
	char *isbn = InputIsbn(f, db, lstCategory, lstBook);
	char *name = InputName("Input Name: ");
	char *author = InputName("Input Author: ");
	int price = InputValue("Input Price: ");
	Category *tempCate = SelectedGenres(f, db, lstCategory, lstBook);
	Category cate;
	cate.id = tempCate->id;
	cate.name = tempCate->name;
	cate.status = 1;
	char *publisher = InputName("Input Publisher: ");
	Date date = InputDate();
	int status = 1;

	BOOK *newBook = createBook(isbn, name, author, price, cate, publisher, date, status);
	BOOK *pos = PosID(lstBook, posIsbn);
	addAfter(lstBook, pos, newBook);

	AuthorStatus(f, lstBook, status, author, "wt", 0);
	Output(lstBook);

	return 0;
}

void AddSongView(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	char *newSong;
	char c;
	int flag = 1;
	do
	{
		flag = 1;
		while (flag)
		{
			newSong = new char[MAXLINE];
			OutputTitle("ADD SONG");
			flag = AddSong(f, db, lstCategory, lstBook);
		}
		Notify(4);
		Notify(1);
	} while (ExitAdmin(f, db, lstCategory, lstBook) == 0);
}

void AdminView(FILE *f, Database db, ListCategory lstCategory, ListBook lstBook)
{
	system("cls");
	int opt = 0;
	AdminMenu(opt);

	switch (opt)
	{
	case 49:
		ShowView(f, db, lstCategory, lstBook);
		break;
	case 50:
		FeatureTwo(f, db, lstCategory, lstBook);
		break;
	case 51:
		FeatureThree(f, db, lstCategory, lstBook);
		break;
	case 52:
		FeatureFour(f, db, lstCategory, lstBook);
		break;
	case 53:
		FeatureFive(f, db, lstCategory, lstBook);
		break;
	case 54:
		FeatureSix(f, db, lstCategory, lstBook);
		break;
	case 55:
		AddSongView(f, db, lstCategory, lstBook);
		break;
	case 27:
		exit(0);
	}
}

void main()
{
	SetConsoleCP(437);
	SetConsoleOutputCP(437);

	ListCategory lstCategory;
	Init(lstCategory);
	ListBook lstBook;
	Init(lstBook);
	Database db;
	FILE *f = NULL;

	adminPointer = AdminView;

	Input(f, db, fileNameCate, mode);
	LoadCate(lstCategory, db);
	Input(f, db, fileNameBook, mode);
	LoadBook(lstBook, lstCategory, db);

	AdminView(f, db, lstCategory, lstBook);

	_getch();
}