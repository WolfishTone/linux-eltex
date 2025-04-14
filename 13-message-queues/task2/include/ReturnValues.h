#pragma once

enum ReturnValues {
	success = 0,
	invalid_user_id, NULL_ptr, malloc_err,
	user_exists, user_list_overflow,
	cant_create_file, cant_create_queue, cant_open_queue, cant_delete_queue,
	msgsnd_error,
	msgrcv_error,
	cant_get_queue_stat, user_already_login, user_doesnt_exist,
	invalid_term_size
};


