�d�q�ؖ����擾�T���v��
================

���̃f�B���N�g���ɂ́A�d�q�ؖ����擾�̃N���C�A���g�����
�T�[�r�X�̃T���v���R�[�h���i�[����Ă��܂��B

OpenSOAP API �C���X�g�[����́Amake -f GetCert.mak �ŁA
�R���p�C���ł��A�ȉ��̃t�@�C�����쐬����܂��B

* GetCertClient
  �d�q�ؖ����擾�N���C�A���g�v���O�����ł��B
* GetCertService
  �W�����o�͂�p�����d�q�ؖ����擾�T�[�r�X�v���O�����ł��Binetd ��p���邱�ƂŁA
  socket type �̃T�[�r�X�ɂ��邱�Ƃ��\�ł��B
* GetCertService.cgi
  CGI �^�̓d�q�ؖ����擾�T�[�r�X�v���O�����ł��B
* CA_Refer
  ���ۂɔF�؋ǃf�[�^�x�[�X�ɃA�N�Z�X���ďؖ����擾����v���O�����ł��B
  GetCertService, GetCostService.cgi ����Ăяo�����v���O�����ł��B

���g�p���@�A�e��ݒ�ɂ���

* GetCertClient(�d�q�ؖ����擾�N���C�A���g�v���O����)
  �v���O�����̈����ɁA���̏��L�Җ��̂�^���ċN�����邱�ƂŁA�d�q�ؖ�����
  ���J�����擾�ł��܂��B���ꂼ��A"result.cert" �� "pubKey_res.pem"
  �Ƃ������O�Ő�������܂��B
  �����̖��̂�ύX�������ꍇ�́A GetCertClient.c ���̂��ꂼ�� 
  RESULT_CERT_FILE_NAME, RESULT_PUBKEY_FILE_NAME �Ƃ�����`��ύX�����
  OK�ł��B
  �܂��A����ꂽ�d�q�ؖ����̏������؂̂��߂ɁA�F�؋ǂ̌��J�����K�v�ƂȂ�
  �܂����A���̃t�@�C���� GetCertClient.c ���� CA_PUBKEY_FILE �Ƃ��Ē�`
  ���Ă���܂��̂ŁA�K�v�ɉ����ďC�����p�������B(����̒�`�́AOpenSOAP
  Server ���C���X�g�[�����ꂽ���ɐݒ肳�����̂𗘗p����l�ɐݒ肳���
  ���܂�)

* GetCertService(�d�q�ؖ����擾�T�[�r�X�v���O����)
  ���ۂɌ��������s����v���O����(CA_Refer)���C���X�g�[������Ă���f�B��
  �N�g���Ƃ��āAGetCertService.c ���� REFER_PROGRAM_DIR �Ƃ��Ē�`���Ă�
  �܂��BCA_Refer �𑼂̃f�B���N�g���ɃC���X�g�[�������ꍇ�́A���̕�����
  �ύX���ĉ������B

* CA_Refer(�F�؋ǃf�[�^�x�[�X�ł̒T���v���O����)
  �ݒ�ύX�́A DBRefer.c ���̈ȉ��̒�`�𑀍삵�܂��B

  CA_DATABASE_FILE : �F�؋�DB�t�@�C���̃C���X�g�[����̃t���p�X
  CA_PRIVKEY_FILE  : �F�؋ǂ̔閧���t�@�C����

�Ȃ��A���̃f�B���N�g�����ɃT���v���̔F�؋ǃf�[�^�x�[�X(CA.db)��u����
���܂��B
���̃f�[�^�x�[�X�ɂ́A�ȉ��̏��L�Җ��̂ɑΉ�������J�����i�[����Ă��܂��B
(�R��)

"OpenSOAP-SAMPLE-CA" (�F�؋ǎ��g������)
"David Beckham"
"Alessandro Del Piero"

����́ARegist �T���v���ō쐬�������̂Ɠ���ƂȂ�܂��B

                                                                        �ȏ�