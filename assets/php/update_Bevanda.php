<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$bevanda_old = mysqli_real_escape_string($conn, $_POST['ID_Bevanda_old']);
		$bevanda_new = mysqli_real_escape_string($conn, $_POST['ID_Bevanda_new']);
		$nome_new = mysqli_real_escape_string($conn, $_POST['nome_new']);
		$prezzo_new = mysqli_real_escape_string($conn, $_POST['prezzo_new']);

		$sql = "UPDATE bevanda SET ID_Bevanda='$bevanda_new', Nome='$nome_new', prezzo_EUR='$prezzo_new' WHERE ID_Bevanda='$bevanda_old';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>