/**
 *\file life.cpp
 *\author Alisa Timofeeva
 *\version 1.0
 *\date May 2022
 *\details Описания и реализация в этом файле, библиотеки, для Visual Studio 2019, 
 * для работы подключить библиотеку SFML
 */

/**
 *\mainpage Игра Жизнь
 *\details Программная реализация алгоритма Конвея игры "Жизнь".
 * В программе создаётся вселенная игры - плоскость, разбитая на клетки.
 * Каждой клетке может соответствовать одно из двух состояний -
 * живая бактерия или вода.
 * Создана модель развития жизни популяции бактерий, которую можно 
 * наглядно увидеть в окне вывода.
*/

#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

/**Ширина клеточного поля*/
const float MH = 65.0;		 
/**Высота клеточного поля*/
const float MW = 60.0;
/**Размер клетки в пикселях*/
#define POINT_SIZE 10

/**
 *\struct point 
 *\details Хранит сотояния клетки - "живая" или "мёртвая"
*/
struct point
{
	unsigned is_live : 1;
};

/**
 *\param w окно вывода
 *\param texture текстура воды
 *\param matrix матрица водоёма
 *\details Отрисовка водоёма после каждого шага
*/
void waterDraw(RenderWindow& w, Texture &texture, point **matrix)
{
	/**создание спрайта (текстура+форма) первые два параметра - откуда отсчёт,
	  *вторые два параметра - сколько на сколько вырезаем
	*/
	Sprite sprite0(texture, IntRect(0, 0, POINT_SIZE, POINT_SIZE));
	Sprite sprite1(texture, IntRect(POINT_SIZE, 0, POINT_SIZE, POINT_SIZE));
	/**циклом проходим по массиву точек*/
	for (int i = 0; i < MW; i++)
	{
		for (int j = 0; j < MH; j++)
		{
			/**если точка мёртвая, то выводим клетку водоёма*/
			if (matrix[i][j].is_live == 0)
			{
				sprite0.setPosition(j * (float)POINT_SIZE, i * (float)POINT_SIZE);
				w.draw(sprite0);
			}
			/**если точка живая, то выводим бактерию*/
			else
			{
				sprite1.setPosition(j * (float)POINT_SIZE, i * (float)POINT_SIZE);
				w.draw(sprite1);
			}
		}
	}
}

/**
 *\param matrix матрица водоёма
 *\details Вся матрица заполняется нулями, бактерий нет, водоём пуст.
*/
void fGeneration(point **matrix)
{
	int i, j;
	/**всё заполняем нулями (нет живых)*/
	for (i = 0; i < MW; i++)
	{
		for (j = 0; j < MH; j++)
		{
			matrix[i][j].is_live = 0;
		}
	}
}

/**
 *\param nb массив для результата
 *\param x координата клетки (столбец в матрице)
 *\param y координата клетки (строка в матрице)
 *\details Поиск живых и мёртвых соседей вокруг бактерии. 
 * У клетки восемь соседей. В массив nb по очереди записываются в первую строку - живые бактерии(1), 
 * во вторую - мёртвые(0).
*/
void neighborPoint(int nb[][2], int x, int y)
{
	int i, j;
	int k = 0;
	for (i = x - 1; i <= x + 1; i++)
	{
		for (j = y - 1; j <= y + 1; j++)
		{
			if (i == x && j == y)
			{
				continue;
			}
			nb[k][0] = i;
			nb[k][1] = j;
			k++;
		}
	}
}

/**
 *\param matrix матрица водоёма
 *\param x координата клетки (столбец в матрице)
 *\param y координата клетки (строка в матрице)
 *\return num количество живых соседей клетки(1)
 *\details Подсчёт количества живых соседей вокруг бактерии
*/
unsigned int neighborLive(point **matrix, int x, int y)
{
	int num = 0;
	int i;
	int nb[8][2];
	int x1, y1;
	neighborPoint(nb, x, y);
	for (i = 0; i < 8; i++) {
		x1 = nb[i][0];
		y1 = nb[i][1];
		if (x1 < 0 || y1 < 0)
		{
			continue;
		}
		if (x1 >= MW || y1 >= MH)
		{
			continue;
		}
		if (matrix[x1][y1].is_live == 1)
		{
			num++;
		}
	}
	return num;
}

/**
 *\param m  матрица point
 *\param m1 матрица point
 *\details Обмен значения матриц между собой без копирования, используя указатели.
*/
void swapM(point** &m, point** &m1)
{
	point* tmp;
	for (int i = 0; i < MW; i++) {
		tmp = m[i];
		m[i] = m1[i];
		m1[i] = tmp;
	}
}

/**
 *\param matrix  матрица текущего состояния водоёма
 *\param matrix1 матрица водоёма во время следущего цикла жизни
 *\return isOpt  флаг остановки цикла жизни
 *\details Генерирование следущего поколения обитателей водоёма, с учётом смерти старых и 
 * рождения новых бактерий.
*/
bool nextGeneration(point** matrix, point** matrix1)
{
	/**Флаг остановки активирован. Если ничего не произойдёт - игра остановится*/
	bool isOpt = true;
	int i, j;
	/**Счётчик количества живых бактерий*/
	int liveNb;
	/**Проверяем каждую клетку водоёма*/
	for (i = 0; i < MW; i++)
	{
		for (j = 0; j < MH; j++)
		{
			/**Количество живых бактерий вокруг текущей клетки*/
			liveNb = neighborLive(matrix, i, j);
			/**Если проверяется пустая клетка водоёма*/
			if (matrix[i][j].is_live == 0)
			{
				/**Если количество соседей у этой клетки равно трём, то в ней зарождается жизнь*/
				if (liveNb == 3)
				{
					matrix1[i][j].is_live = 1;
				}
				/**Иначе клетка остаётся пустой*/
				else
				{
					matrix1[i][j].is_live = matrix[i][j].is_live;
				}
			}
			/**Если проверяется живая клетка водоёма*/
			else
			{
				/**Если количество её соседей меньше двух или больше трёх, то она погибает*/
				if (liveNb < 2 || liveNb > 3)
				{
					matrix1[i][j].is_live = 0;
				}
				/**Иначе клетка продолжает жить*/
				else
				{
					matrix1[i][j].is_live = matrix[i][j].is_live;
				}
			}
			/**Если состояние клетки водоёма изменилось, то игра будет продолжаться*/
			if (matrix1[i][j].is_live != matrix[i][j].is_live)
			{
				isOpt = false;
			}
		}
	}
	/**Новая матрица становится текущей. Переход к следущему поколению*/
	swapM(matrix, matrix1);
	return isOpt;
}

/**
 *\details Вывод окна с информацией об игре и её правилами. 
*/
void help()
{
	/**Создаём объект окна*/
	RenderWindow helpW(VideoMode(550, 600), " ", Style::None);
	/**Главный цикл окна. Выполняется пока окно открыто.*/
	while (helpW.isOpen())
	{
		/**Переменная событий*/
		Event e;
		/**Создаём текстуру "крестика" для закрытия окна*/
		Texture tClose;
		/**Загружаем текстуру из файла*/
		tClose.loadFromFile("picture\\close.png");
		/**Сглаживаем формы текстуры*/
		tClose.setSmooth(true);
		/**координаты левого верхнего угла кнопки "крестик"*/
		int closeSpritePositionX = 501;
		int closeSpritePositionY = 4;
		/**размер кнопки "крестик" в пиклеслях*/
		int closeSizeX = 45;
		int closeSizeY = 45;
		/**Отслеживание текущих координат мыши*/
		Vector2i posMouse = Mouse::getPosition(helpW);
		/**Обработка событий в окне*/
		while (helpW.pollEvent(e))
		{
			/**отслеживание нажатий мыши*/
			if (e.type == Event::MouseButtonPressed)
			{
				/**если нажата левая кнопка мыши*/
				if (e.key.code == Mouse::Left)
				{
					/**на "крестике" => закрываем окно*/
					if ((posMouse.x >= closeSpritePositionX) && (posMouse.x <= closeSpritePositionX + closeSizeX)
						&& (posMouse.y >= closeSpritePositionY) && (posMouse.y <= closeSpritePositionY + closeSizeY))
					{
						helpW.close();
					}
				}
			}
		}
		/**создаём и загружаем шрифт из файла*/
		Font font;
		font.loadFromFile("fonts\\arial.ttf");
		/**создаём основной текст*/
		Text formName(L"Игра «Жизнь» — это клеточный автомат, придуманный английским \nматематиком Джоном Конвеем в 1970 году. \nМесто действия этой игры — это размеченная на клетки плоскость. \nКаждая клетка на этой поверхности может находиться в двух \nсостояниях: быть «живой» или быть «мёртвой». Клетка имеет восемь \nсоседей, окружающих её. \nРасстановка живых клеток в начале игры называется первым поколением. \nКаждое следующее поколение рассчитывается на основе предыдущего \nпо таким правилам: \n- в пустой клетке, рядом с которой ровно три живые клетки, \nзарождается жизнь; \n- если у живой клетки есть две или три живые соседки, то эта клетка \nпродолжает жить; в противном случае, если соседей меньше двух \nили больше трёх, клетка умирает;\n- Игра прекращается, если:\n\t- на поле не останется ни одной «живой» клетки;\n\t- складывается периодическая конфигурация;\n\t- складывается стабильная конфигурация.\n\nИгрок не принимает прямого участия в игре, а лишь расставляет \nили генерирует начальную конфигурацию «живых» клеток, которые затем \nвзаимодействуют согласно правилам уже без его участия \n(он является наблюдателем).\n\n\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t© Алиса Тимофеева 2022",
			font, 15);
		/**устанавливаем цвет основного текста*/
		formName.setFillColor(Color::Black);
		/**курсив*/
		formName.setStyle(Text::Italic);
		/**устанавливаем позицию текста в окне*/
		formName.setPosition(19, 140);
		/**создаём заголовок*/
		Text fName(L"Игра «Жизнь»", font, 45);
		fName.setFillColor(Color(54, 101, 169));
		/**курсив и жирность*/
		fName.setStyle(Text::Italic | Text::Bold);
		/**устанавливаем позицию заголовка в окне*/
		fName.setPosition(135, 55);
		/**заполняем окно цветом фона*/
		helpW.clear(Color(235, 241, 251));
		Sprite sClose(tClose);
		/**если мышь наведена на "крестик" => загружаем вторую часть текстуры*/
		if ((posMouse.x >= closeSpritePositionX) && (posMouse.x <= closeSpritePositionX + closeSizeX)
			&& (posMouse.y >= closeSpritePositionY) && (posMouse.y <= closeSpritePositionY + closeSizeY))
		{
			sClose.setTextureRect(IntRect(closeSizeX, 0, closeSizeX, closeSizeY));
		}
		/**если мышь не наведена на "крестик" => загружаем первую часть текстуры*/
		else
		{
			sClose.setTextureRect(IntRect(0, 0, closeSizeX, closeSizeY));
		}
		/**устанавливаем координату левого верхнего угла кнопки "крестик"*/
		sClose.setPosition((float)closeSpritePositionX, (float)closeSpritePositionY);
		/**Отрисовываем все элементы окна*/
		helpW.draw(sClose);
		helpW.draw(formName);
		helpW.draw(fName);
		/**отображаем окно*/
		helpW.display();
	}
}

/**
 *\param matrix матрица водоёма
 *\details Очищаем водоём от бактерий
*/
void clear(point** matrix)
{
	for (int i = 0; i < MW; i++)
	{
		for (int j = 0; j < MH; j++)
		{
			matrix[i][j].is_live = 0;
		}
	}
}

/**
 *\param matrix матрица водоёма
 *\param button номер кнопки стандартной конфигурации колонии
 *\details Очищает водоём от всех старых клеток. Заселяет в водоём устойчивую колонию бактерий
*/
void button(point** matrix, int button)
{
	/**очистка водоёма*/
	clear(matrix);
	/**выбор конфигурации*/
	switch (button)
	{
	/**глайдер*/
	case 1:
		matrix[29][31].is_live = 1;
		matrix[30][32].is_live = 1;
		matrix[31][30].is_live = 1; matrix[31][31].is_live = 1; matrix[31][32].is_live = 1;
		break;
	/**звездолёт*/
	case 2:
		matrix[25][30].is_live = 1; matrix[25][33].is_live = 1;
		matrix[26][34].is_live = 1;
		matrix[27][30].is_live = 1; matrix[27][34].is_live = 1;
		matrix[28][31].is_live = 1; matrix[28][32].is_live = 1;
		matrix[28][33].is_live = 1; matrix[28][34].is_live = 1;
		break;
	/**база глайдеров*/
	case 3:
		matrix[26][31].is_live = 1; matrix[26][32].is_live = 1;
		matrix[26][33].is_live = 1; matrix[26][34].is_live = 1;
		matrix[28][29].is_live = 1; matrix[28][30].is_live = 1;
		matrix[28][31].is_live = 1; matrix[28][32].is_live = 1;
		matrix[28][33].is_live = 1; matrix[28][34].is_live = 1;
		matrix[28][35].is_live = 1; matrix[28][36].is_live = 1;
		matrix[30][29].is_live = 1; matrix[30][30].is_live = 1;
		matrix[30][31].is_live = 1; matrix[30][32].is_live = 1;
		matrix[30][33].is_live = 1; matrix[30][34].is_live = 1;
		matrix[30][35].is_live = 1; matrix[30][36].is_live = 1;
		matrix[30][28].is_live = 1; matrix[30][37].is_live = 1;
		matrix[32][29].is_live = 1; matrix[32][30].is_live = 1;
		matrix[32][31].is_live = 1; matrix[32][32].is_live = 1;
		matrix[32][33].is_live = 1; matrix[32][34].is_live = 1;
		matrix[32][35].is_live = 1; matrix[32][36].is_live = 1;
		matrix[34][31].is_live = 1; matrix[34][32].is_live = 1;
		matrix[34][33].is_live = 1; matrix[34][34].is_live = 1;
		break;

	/**пентаполе*/
	case 4:
		matrix[26][28].is_live = 1; matrix[26][29].is_live = 1;
		matrix[27][28].is_live = 1; 
		matrix[28][29].is_live = 1; matrix[28][31].is_live = 1;
		matrix[30][31].is_live = 1; matrix[30][33].is_live = 1;
		matrix[32][33].is_live = 1; matrix[32][35].is_live = 1;
		matrix[33][34].is_live = 1; matrix[33][35].is_live = 1;
		break;
	//феникс
	case 5:
		matrix[26][31].is_live = 1; 
		matrix[27][31].is_live = 1; matrix[27][33].is_live = 1;
		matrix[28][29].is_live = 1; 
		matrix[29][34].is_live = 1; matrix[29][35].is_live = 1; 
		matrix[30][28].is_live = 1; matrix[30][29].is_live = 1; 
		matrix[31][34].is_live = 1;
		matrix[32][30].is_live = 1; matrix[32][32].is_live = 1;
		matrix[33][32].is_live = 1;
		break;
	/**часы*/
	case 6:
		matrix[24][32].is_live = 1; matrix[24][33].is_live = 1;
		matrix[25][32].is_live = 1; matrix[25][33].is_live = 1;
		matrix[27][30].is_live = 1; matrix[27][31].is_live = 1;
		matrix[27][32].is_live = 1; matrix[27][33].is_live = 1;
		matrix[28][26].is_live = 1; matrix[28][27].is_live = 1;
		matrix[29][26].is_live = 1; matrix[29][27].is_live = 1;
		matrix[28][29].is_live = 1; matrix[28][32].is_live = 1;
		matrix[28][34].is_live = 1; matrix[29][34].is_live = 1; 
		matrix[29][29].is_live = 1; matrix[29][31].is_live = 1;
		matrix[30][36].is_live = 1; matrix[30][37].is_live = 1;
		matrix[31][36].is_live = 1; matrix[31][37].is_live = 1;
		matrix[30][29].is_live = 1; matrix[30][31].is_live = 1;
		matrix[30][34].is_live = 1; 
		matrix[31][29].is_live = 1; matrix[31][34].is_live = 1;
		matrix[32][30].is_live = 1; matrix[32][31].is_live = 1;
		matrix[32][32].is_live = 1; matrix[32][33].is_live = 1;
		matrix[34][30].is_live = 1; matrix[34][31].is_live = 1;
		matrix[35][30].is_live = 1; matrix[35][31].is_live = 1;
		break;
	/**бриллиант*/
	case 7:
		matrix[23][31].is_live = 1; matrix[24][30].is_live = 1;
		matrix[24][32].is_live = 1; matrix[25][29].is_live = 1;
		matrix[25][31].is_live = 1; matrix[25][33].is_live = 1;
		matrix[26][29].is_live = 1; matrix[26][33].is_live = 1;
		matrix[27][27].is_live = 1; matrix[27][28].is_live = 1;
		matrix[27][31].is_live = 1; matrix[27][34].is_live = 1;
		matrix[27][35].is_live = 1; matrix[28][26].is_live = 1;
		matrix[28][31].is_live = 1; matrix[28][36].is_live = 1;
		matrix[29][25].is_live = 1; matrix[29][27].is_live = 1;
		matrix[29][29].is_live = 1; matrix[29][30].is_live = 1;
		matrix[29][32].is_live = 1; matrix[29][33].is_live = 1;
		matrix[29][35].is_live = 1; matrix[29][37].is_live = 1;
		matrix[30][26].is_live = 1; matrix[30][31].is_live = 1;
		matrix[30][36].is_live = 1; matrix[31][27].is_live = 1;
		matrix[31][28].is_live = 1; matrix[31][31].is_live = 1;
		matrix[31][34].is_live = 1; matrix[31][35].is_live = 1;
		matrix[32][29].is_live = 1; matrix[32][33].is_live = 1;
		matrix[33][29].is_live = 1; matrix[33][31].is_live = 1;
		matrix[33][33].is_live = 1; matrix[34][30].is_live = 1;
		matrix[34][32].is_live = 1; matrix[35][31].is_live = 1;
		break;
	/**звезда*/
	case 8:
		matrix[24][31].is_live = 1; matrix[25][30].is_live = 1;
		matrix[25][31].is_live = 1; matrix[25][32].is_live = 1;
		matrix[26][28].is_live = 1; matrix[26][29].is_live = 1;
		matrix[26][30].is_live = 1; matrix[26][32].is_live = 1;
		matrix[26][33].is_live = 1; matrix[26][34].is_live = 1;
		matrix[27][28].is_live = 1; matrix[27][34].is_live = 1;
		matrix[28][27].is_live = 1; matrix[28][28].is_live = 1;
		matrix[28][34].is_live = 1; matrix[28][35].is_live = 1;
		matrix[29][26].is_live = 1; matrix[29][27].is_live = 1;
		matrix[29][35].is_live = 1; matrix[29][36].is_live = 1;
		matrix[30][27].is_live = 1; matrix[30][28].is_live = 1;
		matrix[30][34].is_live = 1; matrix[30][35].is_live = 1;
		matrix[31][28].is_live = 1; matrix[31][34].is_live = 1;
		matrix[32][28].is_live = 1; matrix[32][29].is_live = 1;
		matrix[32][30].is_live = 1; matrix[32][32].is_live = 1;
		matrix[32][33].is_live = 1; matrix[32][34].is_live = 1;
		matrix[33][30].is_live = 1; matrix[33][31].is_live = 1;
		matrix[33][32].is_live = 1; matrix[34][31].is_live = 1;
		break;
	/**галактика*/
	case 9:
		matrix[25][27].is_live = 1; matrix[25][28].is_live = 1;
		matrix[25][29].is_live = 1; matrix[25][30].is_live = 1;
		matrix[25][31].is_live = 1; matrix[25][32].is_live = 1;
		matrix[26][27].is_live = 1; matrix[26][28].is_live = 1;
		matrix[26][29].is_live = 1; matrix[26][30].is_live = 1;
		matrix[26][31].is_live = 1; matrix[26][32].is_live = 1;
		matrix[28][27].is_live = 1; matrix[29][27].is_live = 1;
		matrix[30][27].is_live = 1; matrix[31][27].is_live = 1;
		matrix[32][27].is_live = 1; matrix[33][27].is_live = 1;
		matrix[28][28].is_live = 1; matrix[29][28].is_live = 1;
		matrix[30][28].is_live = 1; matrix[31][28].is_live = 1;
		matrix[32][28].is_live = 1; matrix[33][28].is_live = 1;
		matrix[25][34].is_live = 1; matrix[26][34].is_live = 1;
		matrix[27][34].is_live = 1; matrix[28][34].is_live = 1;
		matrix[29][34].is_live = 1; matrix[30][34].is_live = 1;
		matrix[25][35].is_live = 1; matrix[26][35].is_live = 1;
		matrix[27][35].is_live = 1; matrix[28][35].is_live = 1;
		matrix[29][35].is_live = 1; matrix[30][35].is_live = 1;
		matrix[32][30].is_live = 1; matrix[32][31].is_live = 1;
		matrix[32][32].is_live = 1; matrix[32][33].is_live = 1;
		matrix[32][34].is_live = 1; matrix[32][35].is_live = 1;
		matrix[33][30].is_live = 1; matrix[33][31].is_live = 1;
		matrix[33][32].is_live = 1; matrix[33][33].is_live = 1;
		matrix[33][34].is_live = 1; matrix[33][35].is_live = 1;
		break;
	/**тумблер*/
	case 10:
		matrix[26][29].is_live = 1; matrix[26][30].is_live = 1;
		matrix[26][32].is_live = 1; matrix[26][33].is_live = 1;
		matrix[28][30].is_live = 1; matrix[28][32].is_live = 1;
		matrix[29][30].is_live = 1; matrix[29][32].is_live = 1;
		matrix[29][27].is_live = 1; matrix[29][28].is_live = 1;
		matrix[29][34].is_live = 1; matrix[29][35].is_live = 1;
		matrix[30][27].is_live = 1; matrix[30][28].is_live = 1;
		matrix[30][34].is_live = 1; matrix[30][35].is_live = 1;
		matrix[30][29].is_live = 1; matrix[30][33].is_live = 1;
		break;
	/**лилия*/
	case 11:
		matrix[24][31].is_live = 1; matrix[25][30].is_live = 1;
		matrix[25][32].is_live = 1; matrix[26][29].is_live = 1;
		matrix[26][31].is_live = 1; matrix[26][33].is_live = 1;
		matrix[27][29].is_live = 1; matrix[27][31].is_live = 1;
		matrix[27][33].is_live = 1; matrix[28][26].is_live = 1;
		matrix[28][27].is_live = 1; matrix[28][29].is_live = 1;
		matrix[28][30].is_live = 1; matrix[28][31].is_live = 1;
		matrix[28][32].is_live = 1; matrix[28][33].is_live = 1;
		matrix[28][35].is_live = 1; matrix[28][36].is_live = 1;
		matrix[29][26].is_live = 1; matrix[29][28].is_live = 1;
		matrix[29][34].is_live = 1; matrix[29][36].is_live = 1;
		matrix[30][29].is_live = 1; matrix[30][30].is_live = 1;
		matrix[30][31].is_live = 1; matrix[30][32].is_live = 1;
		matrix[30][33].is_live = 1; matrix[32][31].is_live = 1;
		matrix[33][30].is_live = 1; matrix[33][32].is_live = 1;
		matrix[34][31].is_live = 1;
		break;
	/**R2-D2*/
	case 12:
		matrix[24][31].is_live = 1; matrix[24][32].is_live = 1;
		matrix[25][31].is_live = 1; matrix[25][32].is_live = 1;
		matrix[27][29].is_live = 1; matrix[27][30].is_live = 1;
		matrix[27][31].is_live = 1; matrix[27][32].is_live = 1;
		matrix[27][33].is_live = 1; matrix[27][34].is_live = 1;
		matrix[28][28].is_live = 1; matrix[28][35].is_live = 1;
		matrix[29][28].is_live = 1; matrix[29][29].is_live = 1;
		matrix[29][32].is_live = 1; matrix[29][33].is_live = 1;
		matrix[29][34].is_live = 1; matrix[29][35].is_live = 1;
		matrix[31][28].is_live = 1; matrix[31][29].is_live = 1;
		matrix[31][30].is_live = 1; matrix[31][31].is_live = 1;
		matrix[31][32].is_live = 1; matrix[31][33].is_live = 1;
		matrix[31][34].is_live = 1; matrix[31][35].is_live = 1;
		matrix[32][28].is_live = 1; matrix[32][35].is_live = 1;
		matrix[33][31].is_live = 1; matrix[33][32].is_live = 1;
		matrix[34][31].is_live = 1; matrix[34][32].is_live = 1;
		break;
	}
}

/**
 *\param matrix матрица водоёма
 *\details Выводит окна для выбора стандартной конфигурации колонии
*/
void figures(point** matrix)
{
	/**создание окна*/
	RenderWindow figW(VideoMode(800, 600), " ", Style::None);
	/**устанавливаем координату левого верхнего угла кнопки "крестик"*/
	int closeSpritePositionX = 751;
	int closeSpritePositionY = 4;
	/**размер кнопки "крестик" в пиклеслях*/
	int closeSizeX = 45;
	int closeSizeY = 45;
	/**размер 1-12 кнопкок в пиклеслях*/
	int SizeX = 200;
	int SizeY = 173;
	/**позиция по x и y спрайта 1 кнопки*/
	int SpritePositionX1 = 0;
	int SpritePositionY1 = 81;
	/**позиция по x и y спрайта 2 кнопки*/
	int SpritePositionX2 = 200;
	int SpritePositionY2 = 81;
	/**позиция по x и y спрайта 3 кнопки*/
	int SpritePositionX3 = 400;
	int SpritePositionY3 = 81;
	/**позиция по x и y спрайта 4 кнопки*/
	int SpritePositionX4 = 600;
	int SpritePositionY4 = 81;
	/**позиция по x и y спрайта 5 кнопки*/
	int SpritePositionX5 = 0;
	int SpritePositionY5 = 254;
	/**позиция по x и y спрайта 6 кнопки*/
	int SpritePositionX6 = 200;
	int SpritePositionY6 = 254;
	/**позиция по x и y спрайта 7 кнопки*/
	int SpritePositionX7 = 400;
	int SpritePositionY7 = 254;
	/**позиция по x и y спрайта 8 кнопки*/
	int SpritePositionX8 = 600;
	int SpritePositionY8 = 254;
	/**позиция по x и y спрайта 9 кнопки*/
	int SpritePositionX9 = 0;
	int SpritePositionY9 = 427;
	/**позиция по x и y спрайта 10 кнопки*/
	int SpritePositionX10 = 200;
	int SpritePositionY10 = 427;
	/**позиция по x и y спрайта 11 кнопки*/
	int SpritePositionX11 = 400;
	int SpritePositionY11 = 427;
	/**позиция по x и y спрайта 12 кнопки*/
	int SpritePositionX12 = 600;
	int SpritePositionY12 = 427;
	/**основной цикл окна*/
	while (figW.isOpen())
	{
		/**событие*/
		Event e;
		/**создаём текстуры*/
		Texture tClose, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12;
		/**загружаем текстуры из файлов*/
		b1.loadFromFile("picture\\glider.png");
		b2.loadFromFile("picture\\spaceship.png");
		b3.loadFromFile("picture\\gliderbase.png");
		b4.loadFromFile("picture\\pentapole.png");
		b5.loadFromFile("picture\\phoenix.png");
		b6.loadFromFile("picture\\clock.png");
		b7.loadFromFile("picture\\diamond.png");
		b8.loadFromFile("picture\\star.png");
		b9.loadFromFile("picture\\galaxy.png");
		b10.loadFromFile("picture\\tumbler.png");
		b11.loadFromFile("picture\\lili.png");
		b12.loadFromFile("picture\\r2d2.png");
		tClose.loadFromFile("picture\\close.png");
		/**сглаживаем формы текстуры*/
		tClose.setSmooth(true);
		b1.setSmooth(true);	b2.setSmooth(true);
		b3.setSmooth(true);	b4.setSmooth(true);
		b5.setSmooth(true);	b6.setSmooth(true);
		b7.setSmooth(true);	b8.setSmooth(true);
		b9.setSmooth(true);	b10.setSmooth(true);
		b11.setSmooth(true);b12.setSmooth(true);
		/**отслеживание положения мыши*/
		Vector2i posMouse = Mouse::getPosition(figW);
		/**проверка событий в окне*/
		while (figW.pollEvent(e))
		{
			/**отслеживание нажатий мыши*/
			if (e.type == Event::MouseButtonPressed)
			{
				/**нажата левая кнопка мыши*/
				if (e.key.code == Mouse::Left)
				{
					/**на "крестике"   => закрываем окно*/
					if ((posMouse.x >= closeSpritePositionX) && (posMouse.x <= closeSpritePositionX + closeSizeX)
						&& (posMouse.y >= closeSpritePositionY) && (posMouse.y <= closeSpritePositionY + closeSizeY))
					{
						figW.close();
					}
					/**на кнопке 1*/
					if ((posMouse.x >= SpritePositionX1) && (posMouse.x <= SpritePositionX1 + SizeX)
						&& (posMouse.y >= SpritePositionY1) && (posMouse.y <= SpritePositionY1 + SizeY))
					{
						button(matrix, 1);
						figW.close();
					}
					/**на кнопке 2*/
					if ((posMouse.x >= SpritePositionX2) && (posMouse.x <= SpritePositionX2 + SizeX)
						&& (posMouse.y >= SpritePositionY2) && (posMouse.y <= SpritePositionY2 + SizeY))
					{
						button(matrix, 2);
						figW.close();
					}
					/**на кнопке 3*/
					if ((posMouse.x >= SpritePositionX3) && (posMouse.x <= SpritePositionX3 + SizeX)
						&& (posMouse.y >= SpritePositionY3) && (posMouse.y <= SpritePositionY3 + SizeY))
					{
						button(matrix, 3);
						figW.close();
					}
					/**на кнопке 4*/
					if ((posMouse.x >= SpritePositionX4) && (posMouse.x <= SpritePositionX4 + SizeX)
						&& (posMouse.y >= SpritePositionY4) && (posMouse.y <= SpritePositionY4 + SizeY))
					{
						button(matrix, 4);
						figW.close();
					}
					/**на кнопке 5*/
					if ((posMouse.x >= SpritePositionX5) && (posMouse.x <= SpritePositionX5 + SizeX)
						&& (posMouse.y >= SpritePositionY5) && (posMouse.y <= SpritePositionY5 + SizeY))
					{
						button(matrix, 5);
						figW.close();
					}
					/**на кнопке 6*/
					if ((posMouse.x >= SpritePositionX6) && (posMouse.x <= SpritePositionX6 + SizeX)
						&& (posMouse.y >= SpritePositionY6) && (posMouse.y <= SpritePositionY6 + SizeY))
					{
						button(matrix, 6);
						figW.close();
					}
					/**на кнопке 7*/
					if ((posMouse.x >= SpritePositionX7) && (posMouse.x <= SpritePositionX7 + SizeX)
						&& (posMouse.y >= SpritePositionY7) && (posMouse.y <= SpritePositionY7 + SizeY))
					{
						button(matrix, 7);
						figW.close();
					}
					/**на кнопке 8*/
					if ((posMouse.x >= SpritePositionX8) && (posMouse.x <= SpritePositionX8 + SizeX)
						&& (posMouse.y >= SpritePositionY8) && (posMouse.y <= SpritePositionY8 + SizeY))
					{
						button(matrix, 8);
						figW.close();
					}
					/**на кнопке 9*/
					if ((posMouse.x >= SpritePositionX9) && (posMouse.x <= SpritePositionX9 + SizeX)
						&& (posMouse.y >= SpritePositionY9) && (posMouse.y <= SpritePositionY9 + SizeY))
					{
						button(matrix, 9);
						figW.close();
					}
					/**на кнопке 10*/
					if ((posMouse.x >= SpritePositionX10) && (posMouse.x <= SpritePositionX10 + SizeX)
						&& (posMouse.y >= SpritePositionY10) && (posMouse.y <= SpritePositionY10 + SizeY))
					{
						button(matrix, 10);
						figW.close();
					}
					/**на кнопке 11*/
					if ((posMouse.x >= SpritePositionX11) && (posMouse.x <= SpritePositionX11 + SizeX)
						&& (posMouse.y >= SpritePositionY11) && (posMouse.y <= SpritePositionY11 + SizeY))
					{
						button(matrix, 11);
						figW.close();
					}
					/**на кнопке 12*/
					if ((posMouse.x >= SpritePositionX12) && (posMouse.x <= SpritePositionX12 + SizeX)
						&& (posMouse.y >= SpritePositionY12) && (posMouse.y <= SpritePositionY12 + SizeY))
					{
						button(matrix, 12);
						figW.close();
					}
				}
			}
		}
		/**создаём и загружаем шрифт*/
		Font font;
		font.loadFromFile("fonts\\arial.ttf");
		/**создаём заголовок*/
		Text fName(L"Фигуры", font, 45);
		/**устанавливаем цвет текста*/
		fName.setFillColor(Color(54, 101, 169));
		/**курсив и жирность*/
		fName.setStyle(Text::Italic | Text::Bold);
		/**установка позиции текста*/
		fName.setPosition(320, 20);
		/**установка цвета фона окна*/
		figW.clear(Color(235, 241, 251));
		/**создаём спрайты из текстур*/
		Sprite sClose(tClose);
		Sprite sb1(b1);	Sprite sb2(b2);	Sprite sb3(b3);	Sprite sb4(b4);
		Sprite sb5(b5);	Sprite sb6(b6);	Sprite sb7(b7);	Sprite sb8(b8);
		Sprite sb9(b9);	Sprite sb10(b10); Sprite sb11(b11);	Sprite sb12(b12);
		/**если мышь наведена на "крестик" => загружаем вторую часть текстуры*/
		if ((posMouse.x >= closeSpritePositionX) && (posMouse.x <= closeSpritePositionX + closeSizeX)
			&& (posMouse.y >= closeSpritePositionY) && (posMouse.y <= closeSpritePositionY + closeSizeY))
		{
			sClose.setTextureRect(IntRect(closeSizeX, 0, closeSizeX, closeSizeY));
		}
		/**если мышь не наведена на "крестик" => загружаем первую часть текстуры*/
		else
		{
			sClose.setTextureRect(IntRect(0, 0, closeSizeX, closeSizeY));
		}
		/**если мышь наведена на 1 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX1) && (posMouse.x <= SpritePositionX1 + SizeX)
			&& (posMouse.y >= SpritePositionY1) && (posMouse.y <= SpritePositionY1 + SizeY))
		{
			sb1.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 1 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb1.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 2 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX2) && (posMouse.x <= SpritePositionX2 + SizeX)
			&& (posMouse.y >= SpritePositionY2) && (posMouse.y <= SpritePositionY2 + SizeY))
		{
			sb2.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 2 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb2.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 3 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX3) && (posMouse.x <= SpritePositionX3 + SizeX)
			&& (posMouse.y >= SpritePositionY3) && (posMouse.y <= SpritePositionY3 + SizeY))
		{
			sb3.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 3 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb3.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 4 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX4) && (posMouse.x <= SpritePositionX4 + SizeX)
			&& (posMouse.y >= SpritePositionY4) && (posMouse.y <= SpritePositionY4 + SizeY))
		{
			sb4.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 4 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb4.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 5 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX5) && (posMouse.x <= SpritePositionX5 + SizeX)
			&& (posMouse.y >= SpritePositionY5) && (posMouse.y <= SpritePositionY5 + SizeY))
		{
			sb5.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 5 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb5.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 6 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX6) && (posMouse.x <= SpritePositionX6 + SizeX)
			&& (posMouse.y >= SpritePositionY6) && (posMouse.y <= SpritePositionY6 + SizeY))
		{
			sb6.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 6 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb6.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 7 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX7) && (posMouse.x <= SpritePositionX7 + SizeX)
			&& (posMouse.y >= SpritePositionY7) && (posMouse.y <= SpritePositionY7 + SizeY))
		{
			sb7.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 7 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb7.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 8 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX8) && (posMouse.x <= SpritePositionX8 + SizeX)
			&& (posMouse.y >= SpritePositionY8) && (posMouse.y <= SpritePositionY8 + SizeY))
		{
			sb8.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 8 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb8.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 9 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX9) && (posMouse.x <= SpritePositionX9 + SizeX)
			&& (posMouse.y >= SpritePositionY9) && (posMouse.y <= SpritePositionY9 + SizeY))
		{
			sb9.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 9 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb9.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 10 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX10) && (posMouse.x <= SpritePositionX10 + SizeX)
			&& (posMouse.y >= SpritePositionY10) && (posMouse.y <= SpritePositionY10 + SizeY))
		{
			sb10.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 10 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb10.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 11 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX11) && (posMouse.x <= SpritePositionX11 + SizeX)
			&& (posMouse.y >= SpritePositionY11) && (posMouse.y <= SpritePositionY11 + SizeY))
		{
			sb11.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 11 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb11.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**если мышь наведена на 12 кнопку => загружаем вторую часть текстуры*/
		if ((posMouse.x >= SpritePositionX12) && (posMouse.x <= SpritePositionX12 + SizeX)
			&& (posMouse.y >= SpritePositionY12) && (posMouse.y <= SpritePositionY12 + SizeY))
		{
			sb12.setTextureRect(IntRect(SizeX, 0, SizeX, SizeY));
		}
		/**если мышь не наведена 12 кнопку => загружаем первую часть текстуры*/
		else
		{
			sb12.setTextureRect(IntRect(0, 0, SizeX, SizeY));
		}
		/**утанавливаем позиции вывода спрайтов*/
		sClose.setPosition((float)closeSpritePositionX, (float)closeSpritePositionY);
		sb1.setPosition((float)SpritePositionX1, (float)SpritePositionY1);
		sb2.setPosition((float)SpritePositionX2, (float)SpritePositionY2);
		sb3.setPosition((float)SpritePositionX3, (float)SpritePositionY3);
		sb4.setPosition((float)SpritePositionX4, (float)SpritePositionY4);
		sb5.setPosition((float)SpritePositionX5, (float)SpritePositionY5);
		sb6.setPosition((float)SpritePositionX6, (float)SpritePositionY6);
		sb7.setPosition((float)SpritePositionX7, (float)SpritePositionY7);
		sb8.setPosition((float)SpritePositionX8, (float)SpritePositionY8);
		sb9.setPosition((float)SpritePositionX9, (float)SpritePositionY9);
		sb10.setPosition((float)SpritePositionX10, (float)SpritePositionY10);
		sb11.setPosition((float)SpritePositionX11, (float)SpritePositionY11);
		sb12.setPosition((float)SpritePositionX12, (float)SpritePositionY12);
		/**отрисовываем спрайты в окне*/
		figW.draw(sClose);
		figW.draw(fName); 
		figW.draw(sb1); figW.draw(sb2); figW.draw(sb3);	figW.draw(sb4);
		figW.draw(sb5); figW.draw(sb6);	figW.draw(sb7);	figW.draw(sb8);
		figW.draw(sb9);	figW.draw(sb10); figW.draw(sb11); figW.draw(sb12);
		/**отрисовываем окно*/
		figW.display();
	}
}


/**
 *\details Основная функция программы. 
 *Осуществляется создание и вывод основного окна. 
 *Управляет и согласовывает работу других частей программы.
*/
int main()
{ 
	/**создаём матрицы водоёма*/
	point** matrix = new point * [MW];
	point** matrix1 = new point * [MW];
	for (int i = 0; i < MW; i++)
	{
		matrix[i] = new point[MH];
		matrix1[i] = new point[MH];
	}
	/**заполняем матрицу "водой" (0)*/
	fGeneration(matrix);
	/**флаг состояния работы программы (true - цикл жизни запущен, false - остановлен)*/
	bool starting = false;
	/**создаём главное окно*/
	RenderWindow window(VideoMode(800, 600), "Life!", Style::None);
	/**устанавливаем координату левого верхнего угла кнопки "крестик"*/
	int closeSpritePositionX = 751;
	int closeSpritePositionY = 4;
	/**размер кнопки "крестик" в пиклеслях*/
	int closeSizeX = 45;
	int closeSizeY = 45;
	/**позиция по x и y спрайта кнопки "старт"*/
	int startSpritePositionX = 669;
	int startSpritePositionY = 150;
	/**размер кнопки "старт" в пиклеслях*/
	int startSizeX = 112;
	int startSizeY = 35;
	/**позиция по x и y спрайта "плюс"*/
	int plusSpritePositionX = 751;
	int plusSpritePositionY = 200;
	/**размер кнопки "плюс" в пиклеслях*/
	int plusSizeX = 20;
	int plusSizeY = 20;
	/**позиция по x и y спрайта "минус"*/
	int minusSpritePositionX = 679;
	int minusSpritePositionY = 200;
	/**размер кнопки "минус" в пиклеслях*/
	int minusSizeX = 20;
	int minusSizeY = 20;
	/**позиция по x и y спрайта "очистка"*/
	int clearSpritePositionX = 669;
	int clearSpritePositionY = 235;
	/**размер кнопки "очистка" в пиклеслях*/
	int clearSizeX = 112;
	int clearSizeY = 35;
	/**позиция по x и y спрайта "фигуры"*/
	int figSpritePositionX = 667;
	int figSpritePositionY = 320;
	/**размер кнопки "фигуры" в пиклеслях*/
	int figSizeX = 112;
	int figSizeY = 35;
	/**позиция по x и y спрайта "об игре"*/
	int helpSpritePositionX = 669;
	int helpSpritePositionY = 370;
	/**размер кнопки "об игре" в пиклеслях*/
	int helpSizeX = 112;
	int helpSizeY = 35;
	/**задержка между сменой "поколений" в секундах*/
	double timer = 0;
	/**объект времени для регулирования задержки*/
	Clock clock;
	/**создание и загрузка шрифта*/
	Font font;
	font.loadFromFile("fonts\\arial.ttf");
	/**массив временных задержек в секундах*/
	double delay[10] = {100000.0, 5.0, 2.0, 1.0, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0};
	/**показатель скорости (0-9) является индексом массива delay.
	 *каждой скорости соответсвует задержка
	*/
	int speedF = 5;
	/**создание текстур*/
	Texture texture, tClose, tFig, tStart, tStop, tClear, tPlus, tMinus, tHelp;
	/**заполнение текстур изображениями кнопок*/
	texture.loadFromFile("picture\\12.png");
	tClose.loadFromFile("picture\\close.png");
	tFig.loadFromFile("picture\\fig.png");
	tStart.loadFromFile("picture\\start.png");
	tStop.loadFromFile("picture\\pause.png");
	tClear.loadFromFile("picture\\clear.png");
	tPlus.loadFromFile("picture\\pluse.png");
	tMinus.loadFromFile("picture\\minus.png");
	tHelp.loadFromFile("picture\\help.png");
	/**сглаживание форм текстур кнопок*/
	tClose.setSmooth(true);	tFig.setSmooth(true);
	tStart.setSmooth(true);	tClear.setSmooth(true);
	tStop.setSmooth(true);	tPlus.setSmooth(true);
	tMinus.setSmooth(true);	tHelp.setSmooth(true);

	/**основной цикл главного окна*/
	while (window.isOpen())
	{
		/**отслеживание положения мыши*/
		Vector2i posMouse = Mouse::getPosition(window);
		/**текущее время таймера в секундах*/
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		/**подсчёт времени прошедшего с последнего обновления водоёма*/
		timer += time;
		/**событие*/
		Event event;
		/**цикл реакций на события*/
		while (window.pollEvent(event))
		{
			/**отслеживание нажатий мыши*/
			if (event.type == Event::MouseButtonPressed)
			{
				/**если была нажата левая кнопка мыши*/
				if (event.key.code == Mouse::Left)
				{
					/**на "крестике"   => закрываем окно*/
					if ((posMouse.x >= closeSpritePositionX) && (posMouse.x <= closeSpritePositionX + closeSizeX)
						&& (posMouse.y >= closeSpritePositionY) && (posMouse.y <= closeSpritePositionY + closeSizeY))
					{
						window.close();
					}
					/**на водоёме  = > меняем состояние клетки*/
					else if ((posMouse.x >= 0) && (posMouse.x <= MH * POINT_SIZE)
						&& (posMouse.y >= 0) && (posMouse.y <= MW * POINT_SIZE))
					{
						int x, y;
						x = posMouse.x / POINT_SIZE;
						y = posMouse.y / POINT_SIZE;
						matrix[y][x].is_live = !matrix[y][x].is_live;
					}
					/**на "очистить" => обнуляем всё значения клеток*/
					else if ((posMouse.x >= clearSpritePositionX) && (posMouse.x <= clearSpritePositionX + clearSizeX)
						&& (posMouse.y >= clearSpritePositionY) && (posMouse.y <= clearSpritePositionY + clearSizeY))
					{
						clear(matrix);
					}
					/**на "старт" => запускаем или останавливаем программу, с помощью флага starting*/
					else if ((posMouse.x >= startSpritePositionX) && (posMouse.x <= startSpritePositionX + startSizeX)
						&& (posMouse.y >= startSpritePositionY) && (posMouse.y <= startSpritePositionY + startSizeY))
					{
						starting = !starting;
					}
					/**на "плюс" = > уменьшаем значение в delay, то есть увеличиваем скорость смены поколений*/
					else if ((posMouse.x >= plusSpritePositionX) && (posMouse.x <= plusSpritePositionX + plusSizeX)
						&& (posMouse.y >= plusSpritePositionY) && (posMouse.y <= plusSpritePositionY + plusSizeY))
					{
						if ((speedF >= 0) && (speedF < 9))
						{
							speedF++;
						}
					}
					/**на "минус" = > увеличиваем значение в delay, то есть уменьшаем скорость смены поколений*/
					else if ((posMouse.x >= minusSpritePositionX) && (posMouse.x <= minusSpritePositionX + minusSizeX)
						&& (posMouse.y >= minusSpritePositionY) && (posMouse.y <= minusSpritePositionY + minusSizeY))
					{
						if ((speedF > 0) && (speedF <= 9))
						{
							speedF--;
						}
					}
					/**на "об игре" => открываем окно с информаций об игре и правилами*/
					else if ((posMouse.x >= helpSpritePositionX) && (posMouse.x <= helpSpritePositionX + helpSizeX)
						&& (posMouse.y >= helpSpritePositionY) && (posMouse.y <= helpSpritePositionY + helpSizeY))
					{
						/**делаем главное окно невидимым*/
						window.setVisible(false);
						/**отрисовываем вспомогательное окно*/
						help();
						/**делаем главное окно видимым*/
						window.setVisible(true);
					}
					/**на "фигуры" => открываем окно с заготовками стандартных колоний*/
					else if ((posMouse.x >= figSpritePositionX) && (posMouse.x <= figSpritePositionX + figSizeX)
						&& (posMouse.y >= figSpritePositionY) && (posMouse.y <= figSpritePositionY + figSizeY))
					{
						/**делаем главное окно невидимым*/
						window.setVisible(false);
						/**отрисовываем вспомогательное окно*/
						figures(matrix);
						/**делаем главное окно видимым*/
						window.setVisible(true);
					}
				}
			}
		}
		/**устанавливаем цвет фона главного окна (RGB)*/
		window.clear(Color(235, 241, 251));

		/**если после смены предыдущего поколения прошло больше времени, чем было задано текущей скоростью игры, 
		 *то делаем попытку запустить следущий цикл смены поколений
		*/
		if (timer > delay[speedF])
		{
			/**если нажата кнопка "старт", то запускаем смену поколений*/
			if (starting)
			{
				/**запуск смены поколений. 
				 *проверка необходимости дальнейшей смены поколений
				 *игра остановится, если в водоёме не произошли изменения или водоём пуст
				*/
				if (nextGeneration(matrix, matrix1))
				{
					starting = !starting;
				}
			}
			/**обнуление таймера*/
			timer = 0;
		}

		/**создаём спрайты кнопок*/
		Sprite sClose(tClose); Sprite sFig(tFig);
		Sprite sStart(tStart); Sprite sClear(tClear);
		Sprite sStop(tStop);   Sprite sPlus(tPlus);
		Sprite sMinus(tMinus); Sprite sHelp(tHelp);
		/**если мышь наведена на "крестик" => загружаем вторую часть текстуры*/
		if ((posMouse.x >= closeSpritePositionX) && (posMouse.x <= closeSpritePositionX + closeSizeX)
			&& (posMouse.y >= closeSpritePositionY) && (posMouse.y <= closeSpritePositionY + closeSizeY))
		{
			sClose.setTextureRect(IntRect(closeSizeX, 0, closeSizeX, closeSizeY));
		}
		/**если мышь не наведена на "крестик" => загружаем первую часть текстуры*/
		else
		{
			sClose.setTextureRect(IntRect(0, 0, closeSizeX, closeSizeY));
		}
		/**если мышь наведена на "плюс" => загружаем вторую часть текстуры*/
		if ((posMouse.x >= plusSpritePositionX) && (posMouse.x <= plusSpritePositionX + plusSizeX)
			&& (posMouse.y >= plusSpritePositionY) && (posMouse.y <= plusSpritePositionY + plusSizeY))
		{
			sPlus.setTextureRect(IntRect(plusSizeX, 0, plusSizeX, plusSizeY));
		}
		/**если мышь не наведена на "плюс" => загружаем первую часть текстуры*/
		else
		{
			sPlus.setTextureRect(IntRect(0, 0, plusSizeX, plusSizeY));
		}
		/**если мышь наведена на "минус" => загружаем вторую часть текстуры*/
		if ((posMouse.x >= minusSpritePositionX) && (posMouse.x <= minusSpritePositionX + minusSizeX)
			&& (posMouse.y >= minusSpritePositionY) && (posMouse.y <= minusSpritePositionY + minusSizeY))
		{
			sMinus.setTextureRect(IntRect(minusSizeX, 0, minusSizeX, minusSizeY));
		}
		/**если мышь не наведена на "минус" => загружаем первую часть текстуры*/
		else
		{
			sMinus.setTextureRect(IntRect(0, 0, minusSizeX, minusSizeY));
		}
		/**если мышь наведена на "об игре" => загружаем вторую часть текстуры*/
		if ((posMouse.x >= helpSpritePositionX) && (posMouse.x <= helpSpritePositionX + helpSizeX)
			&& (posMouse.y >= helpSpritePositionY) && (posMouse.y <= helpSpritePositionY + helpSizeY))
		{
			sHelp.setTextureRect(IntRect(helpSizeX, 0, helpSizeX, helpSizeY));
		}
		/**если мышь не наведена на "об игре" => загружаем первую часть текстуры*/
		else
		{
			sHelp.setTextureRect(IntRect(0, 0, helpSizeX, helpSizeY));
		}
		/**если мышь наведена на "фигуры" => загружаем вторую часть текстуры*/
		if ((posMouse.x >= figSpritePositionX) && (posMouse.x <= figSpritePositionX + figSizeX)
			&& (posMouse.y >= figSpritePositionY) && (posMouse.y <= figSpritePositionY + figSizeY))
		{
			sFig.setTextureRect(IntRect(figSizeX, 0, figSizeX, figSizeY));
		}
		/**если мышь не наведена на "фигуры" => загружаем первую часть текстуры*/
		else
		{
			sFig.setTextureRect(IntRect(0, 0, figSizeX, figSizeY));
		}
		/**в зависимости от состояния кнопки стоп/старт меняем ей название на соответствующее - "старт" или "стоп"*/
		if (!starting)
		{
			/**если мышь наведена на "старт" => загружаем вторую часть текстуры*/
			if ((posMouse.x >= startSpritePositionX) && (posMouse.x <= startSpritePositionX + startSizeX)
				&& (posMouse.y >= startSpritePositionY) && (posMouse.y <= startSpritePositionY + startSizeY))
			{
				sStart.setTextureRect(IntRect(startSizeX, 0, startSizeX, startSizeY));
			}
			/**если мышь не наведена на "старт" => загружаем первую часть текстуры*/
			else
			{
				sStart.setTextureRect(IntRect(0, 0, startSizeX, startSizeY));
			}
		}
		else
		{
			/**если мышь наведена на "стоп" => загружаем вторую часть текстуры*/
			if ((posMouse.x >= startSpritePositionX) && (posMouse.x <= startSpritePositionX + startSizeX)
				&& (posMouse.y >= startSpritePositionY) && (posMouse.y <= startSpritePositionY + startSizeY))
			{
				sStop.setTextureRect(IntRect(startSizeX, 0, startSizeX, startSizeY));
			}
			/**если мышь не наведена на "стоп" => загружаем первую часть текстуры*/
			else
			{
				sStop.setTextureRect(IntRect(0, 0, startSizeX, startSizeY));
			}
		}
		/**если мышь наведена на "очистить" => загружаем вторую часть текстуры*/
		if ((posMouse.x >= clearSpritePositionX) && (posMouse.x <= clearSpritePositionX + clearSizeX)
			&& (posMouse.y >= clearSpritePositionY) && (posMouse.y <= clearSpritePositionY + clearSizeY))
		{
			sClear.setTextureRect(IntRect(clearSizeX, 0, clearSizeX, clearSizeY));
		}
		/**если мышь не наведена на "очистить" => загружаем первую часть текстуры*/
		else
		{
			sClear.setTextureRect(IntRect(0, 0, clearSizeX, clearSizeY));
		}
		/**устанавливаем позицию вывода спрайтов кнопок (отсчёт от левого верхнего угла)*/
		sClose.setPosition((float)closeSpritePositionX, (float)closeSpritePositionY);
		sFig.setPosition((float)figSpritePositionX, (float)figSpritePositionY);
		sStart.setPosition((float)startSpritePositionX, (float)startSpritePositionY);
		sStop.setPosition((float)startSpritePositionX, (float)startSpritePositionY);
		sClear.setPosition((float)clearSpritePositionX, (float)clearSpritePositionY);
		sPlus.setPosition((float)plusSpritePositionX, (float)plusSpritePositionY);
		sMinus.setPosition((float)minusSpritePositionX, (float)minusSpritePositionY);
		sHelp.setPosition((float)helpSpritePositionX, (float)helpSpritePositionY);
		/**показатель скорости*/
		Text fName(to_string(speedF), font, 18);
		/**установка цвета показателя скорости*/
		fName.setFillColor(Color(54, 101, 169));
		/**курсив и жирность*/
		fName.setStyle(Text::Bold);
		/**установка позиции показателя скорости (координаты середины между кнопками "+" и "-")*/
		fName.setPosition((plusSpritePositionX + minusSizeX - (float)minusSpritePositionX)/2 + (float)minusSpritePositionX  - 5, (float)minusSpritePositionY);
		/**отрисовка водоёма*/
		waterDraw(window, texture, matrix);
		/**вывод спрайтов кнопок на экран*/
		window.draw(fName);	window.draw(sClose);
		window.draw(sFig);  window.draw(sPlus);
		window.draw(sMinus);window.draw(sClear);
		window.draw(sHelp);
		if (starting)
		{
			window.draw(sStop);
		}
		else
		{
			window.draw(sStart);
		}
		/**отрисовка главного окна*/
		window.display();
	}
	/**удаляем матрицы*/
	for (int i = 0; i < MW; i++)
	{
		delete matrix[i];
		delete matrix1[i];
	}
	delete[] matrix;
	delete[] matrix1;
	return 0;
}