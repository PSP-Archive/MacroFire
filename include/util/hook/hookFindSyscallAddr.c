/* hookFindSyscallAddr.c */

#include "hook_types.h"

void **hookFindSyscallAddr( void **addr )
{
	/*
		���̊֐��́ANID���瓱����郁�����A�h���X����
		���ۂɃV�X�e���R�[����O�n���h���̈ʒu����肷��B
		
		PSPLINK�̃R�[�h���Q�l�ɂ��Ă��܂��B
	*/
	void **cop0_ctrl_register_12;
	struct hook_syscall_table_header *syscall_table;
	void **syscall_entry;
	unsigned int i, total_entries;
	
	if( ! addr ) return NULL;
	
	/*
		CFC0�̓R�v���Z�b�T0�̐��䃌�W�X�^��ǂݍ��ޖ��߂̂悤���B
		�������߂�MFC0������悤�����A������͔ėp���W�X�^��ǂݍ��ޖ��߂炵���A�ʂ̒l���Ԃ��Ă����B
		
		���̃C�����C���A�Z���u���ŁA�R�v���Z�b�T0��12�Ԗڂ̐��䃌�W�X�^��cop0_ctrl_register_12�ϐ��փR�s�[����B
		����12�Ԗڂ̐��䃌�W�X�^�ɂ̓V�X�e���R�[���e�[�u���̃A�h���X���Z�b�g����Ă���͗l�B
	*/
	asm( "cfc0 %0, $12;" : "=r"( cop0_ctrl_register_12 ) );
	
	/* cop0_ctrl_register_12���Q�Ƃ��āA�V�X�e���R�[���e�[�u�����擾 */
	syscall_table = *cop0_ctrl_register_12;
	
	/* �V�X�e���R�[���e�[�u���̃w�b�_���΂��āA�G���g���������擾 */
	syscall_entry = (void **)( (unsigned int)syscall_table + sizeof( struct hook_syscall_table_header ) );
	
	/* �w�b�_���������Z���ăe�[�u���̎c��T�C�Y���G���g�����Ŏ擾 */
	total_entries = ( syscall_table->size - sizeof( struct hook_syscall_table_header ) ) / sizeof( void* );
	
	/* �ړI�̗�O�n���h�����������A�A�h���X��Ԃ� */
	for( i = 0; i < total_entries; i++ ){
		if( syscall_entry[i] == *addr ) return &(syscall_entry[i]);
	}
	
	return NULL;
}
