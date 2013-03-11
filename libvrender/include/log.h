#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Инициализировать логгер (открыть файл для лога) */
int log_init();

/* Напечатать сообщение */
void log_printf(const char *format, ...);

/* Деинициализирвать логгер */
void log_close();

#ifdef __cplusplus
}
#endif

#endif // LOG_H
