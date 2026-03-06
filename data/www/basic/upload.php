<?php
// $target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
// $uploadOk = 1;
// $imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));
// Check if image file is a actual image or fake image

$dir = "./upload/";

// echo "\n";
// echo $_FILES["fileToUpload"]["tmp_name"];
// echo "\n";
// echo $_FILES["fileToUpload"]["name"];
// echo "\n";
if (!file_exists($dir))
  mkdir($dir, 0700);
// echo sizeof($_FILES);
$n_files = 0;
foreach ($_FILES as $f)
{
  // echo $f["tmp_name"];
  if (!empty($f["tmp_name"]) && file_exists($dir) && is_writeable($dir))
  {
    move_uploaded_file($f["tmp_name"], $dir . $f["name"]);
    $n_files++;
  }
}
echo $n_files;
echo " file(s) uploaded\n";
// if (!empty($_FILES["fileToUpload"]["tmp_name"]) && file_exists($dir) && is_writeable($dir))
//   {
//     // move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $dir . $_FILES["fileToUpload"]["name"]);
//     echo "file uploaded\n";
//   }
// else
//   echo "file size too large or upload folder is not writeable\n";
// if(isset($_POST["submit"])) {
//   $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
//   if($check !== false) {
//     echo "File is an image - " . $check["mime"] . ".";
//     $uploadOk = 1;
//   }
//   else {
//     echo "File is not an image.";
//     $uploadOk = 0;
//   }
// }

?>                        
