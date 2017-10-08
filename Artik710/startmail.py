import subprocess

def sendAutostartMail():
    import smtplib
    import string
    USER = '-------------------@gmail.com' //메일 계정, 비밀번호
    PASS = '---------'
    TO = '-----------------@gmail.com' //보낼 메일
    SUBJECT = '[ARTIK710] Autostart Mail' //제목
    ip = subprocess.check_output("hostname -I", shell = True) 	//subprocess를 통해 								 
								//ip에 Artik ip 입력
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
        ), '\r\n') //string 처리

    server = smtplib.SMTP(HOST)
    server.starttls()
    server.login(USER, PASS)
    server.sendmail(FROM, TO, BODY)
    server.quit()

    print 'Success Autostart ip mail.'

if __name__ == "__main__":
    sendAutostartMail()
