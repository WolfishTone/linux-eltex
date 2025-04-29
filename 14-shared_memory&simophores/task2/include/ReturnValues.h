#pragma once

enum ReturnValues {
	success = 0,
	invalid_user_id, NULL_ptr, malloc_err,
	user_exists, user_list_overflow,
	cant_create_file, cant_create_queue, cant_open_queue, cant_delete_queue,/*refactore ME*/
	
	cant_create_sem, cant_open_sem, cant_delete_sem, cant_get_sem_val, cant_set_sem_val, sem_is_blocked, sem_problem,
	cant_create_shm, cant_open_shm, cant_attach_shm, cant_detach_shm, cant_delete_shm,
	snd_error,
	rcv_error,
	cant_get_queue_stat, user_already_login, user_doesnt_exist,
	invalid_term_size
};


