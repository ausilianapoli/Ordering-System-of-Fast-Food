<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$prenotazione = mysqli_real_escape_string($conn, $_POST['ID_Prenotazione']);

		$sql = "DELETE FROM prenotazione WHERE ID_Prenotazione='$prenotazione';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>