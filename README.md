# snake_on_C
Реализация знаменитой игры Змейка на языке С.     
Для отображения игрового поля и объектов на нём использовалась библиотека ncurses.    
Сама змейка как объект реализована с помощью массива, передвижение змейки осуществляется через прогон массива по циклу (от головы змейки до кончика хвоста), в котором координаты предыдущего сегмента тела передаются следующему сегменту.
