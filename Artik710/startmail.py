import subprocess

def sendAutostartMail():
    import smtplib
    import string
    USER = '-------------------@gmail.com' //���� ����, ��й�ȣ
    PASS = '---------'
    TO = '-----------------@gmail.com' //���� ����
    SUBJECT = '[ARTIK710] Autostart Mail' //����
    ip = subprocess.check_output("hostname -I", shell = True) 	//subprocess�� ���� 								 
								//ip�� Artik ip �Է�
    TEXT = 'ip = %s' %ip
    print TEXT
    FROM = USER
    HOST = 'smtp.gmail.com:587'
    BODY = string.join((
        'From: %s' %FROM,
        'To: %s' %TO,
        'Subject: %s' %SUBJECT ,
        '\r\n',
        TEXT,
        ), '\r\n') //string ó��

    server = smtplib.SMTP(HOST)
    server.starttls()
    server.login(USER, PASS)
    server.sendmail(FROM, TO, BODY)
    server.quit()

    print 'Success Autostart ip mail.'

if __name__ == "__main__":
    sendAutostartMail()
