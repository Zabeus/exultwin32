<form id="sendmail" method="post" action="{ACTION}">
	<div><input type="hidden" name="uid" value="{RAND}" /></div>
	<p>Your name: <input type="text" name="{NAME_F}" value="{NAME_T}" />
		Your e-mail: <input type="text" name="{MAIL_F}" value="{MAIL_T}" />
		Subject: <input type="text" name="{SUBJECT_F}" value="{SUBJECT_T}" />
	</p>
	<p><textarea name="{MESSAGE_F}" rows="20" cols="80">{MESSAGE_T}</textarea></p>
	<p>
		{RECAPTCHA}
		<input type="submit" value="Send Message" />
	</p>
</form>
