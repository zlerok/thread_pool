# thread_pool
Incredible thread pool base boost implementation. No time to explain


Минималистичный и кроссплатформенный пул потоков с возможностью указания приоритета выполнения поставленной задачи, позволяющий по требованию: 
1) изменять число рабочих потоков в процессе выполнения; 
2) ограничивать количество поставленных задач, приводя к ожиданию освобождения места для задачи клиентами пула; 
3) останавливаться, дожидаясь завершения всех поставленных задач; 
4) останавливаться форсированно, не дожидаясь выполнения всех задач.