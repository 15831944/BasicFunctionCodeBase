/* dbftp: enumation type definition for FTP commands. */
#ifndef __FTP__H
#define __FTP__H
enum { LS = 0, BINARY, ASCII, DELETE, CD, CONNECT, CLOSE, EXIT, PWD,
        LLS, LDIR, USER, SHELL, IGNORE_COMMAND, READ, WRITE, HELP,
        FTP_COMPLETE=1,  FTP_CONTINUE, FTP_ERROR
};
#endif
extern void 	getpassword(char *),
close_control_connect(),
close_listen_socket(),
close_data_connection();
extern int 	data_msg(char *, int ),
accept_connection(),
get_listen_socket(),
data_msg( char *tmp_buffer, int len),
send_ctrl_msg(char *, int),
send_data_msg( char *tmp_buffer, int len),
flag_connect_to_server(char *, char *),
get_host_reply(),
get_line(),
check_input();
extern void 	func_read(char *),
func_write( char *),
func_binary_mode(),
func_ascii_mode(),
func_close(),
func_connect(char *),
                func_cdir(char *),
func_delete(char *),
func_list( char *),
func_pwd(char *),
func_login( char *),
func_Shell_cmd(char *),
CleanUp(),
getfile( char *fname),
put_file( char *fname);
extern int   	CheckFds(char *),
check_cmd(char *);
