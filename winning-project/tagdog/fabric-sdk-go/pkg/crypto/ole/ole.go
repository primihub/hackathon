package ole

import (
	"bufio"
	"bytes"
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
)

//init, generate pk & sk, and save to local dir
func Setup(prefix string) error {
	command := exec.Command("oabe_setup", "-s", "CP", "-p", prefix)
	command.Stdout = &bytes.Buffer{}
	command.Stderr = &bytes.Buffer{}

	err := command.Run()
	if err != nil {
		fmt.Println(err)
		fmt.Println(command.Stderr.(*bytes.Buffer).String())
		return err
	}
	fmt.Println(command.Stdout.(*bytes.Buffer).String())
	return nil
}

func KeyGen(attributes []string, userName string, prefix string) error {
	attributesLen := len(attributes)
	attributesStr := ""

	attributesStr += attributes[0]
	for i := 1; i < attributesLen; i++ {
		attributesStr += "|" + attributes[i]
	}

	command := exec.Command("oabe_keygen", "-s", "CP", "-p", prefix, "-i", attributesStr, "-o", userName)
	command.Stdout = &bytes.Buffer{}
	command.Stderr = &bytes.Buffer{}

	err := command.Run()
	if err != nil {
		fmt.Println(err)
		fmt.Println(command.Stderr.(*bytes.Buffer).String())
		return err
	}
	fmt.Println(command.Stdout.(*bytes.Buffer).String())
	return nil
}

func Encrypt(data []byte, policy string, prefix string, inputFileName string, outputFileName string) error {
	//write data into file
	filePath := "." //os.Executable()

	_, err := os.Stat(filePath + "/" + inputFileName)
	if !os.IsNotExist(err) {
		err := os.Remove(filePath + "/" + inputFileName)
		if err != nil {
			fmt.Println(err)
			return err
		}
	}
	file, err := os.Create(filePath + "/" + inputFileName)
	if err != nil {
		fmt.Println(err)
		return err
	}
	file, err = os.OpenFile(filePath+"/"+inputFileName, os.O_WRONLY|os.O_CREATE, 0666)
	//close file
	defer func(file *os.File) {
		err := file.Close()
		if err != nil {
			fmt.Println(err)
		}
	}(file)
	//use *Writer with cache to write into file
	write := bufio.NewWriter(file)
	writeString, err := write.WriteString(string(data))
	if err != nil {
		fmt.Print("number of bytes written to file:")
		fmt.Println(writeString)
		fmt.Println(err)
		return err
	}

	//write
	err = write.Flush()
	if err != nil {
		fmt.Println(err)
		return err
	}

	command := exec.Command("oabe_enc", "-s", "CP", "-p", prefix, "-e", policy, "-i", filePath+"/"+inputFileName, "-o", filePath+"/"+outputFileName)
	command.Stdout = &bytes.Buffer{}
	command.Stderr = &bytes.Buffer{}

	err = command.Run()
	if err != nil {
		fmt.Println(err)
		fmt.Println(command.Stderr.(*bytes.Buffer).String())
		return err
	}
	fmt.Println(command.Stdout.(*bytes.Buffer).String())
	return nil
}

func Decrypt(ciphertextFileName string, userName string, prefix string, plaintextFileName string) error {
	//write data into file
	filePath := "." //os.Executable()

	command := exec.Command("oabe_dec", "-s", "CP", "-p", prefix, "-k", userName+".key", "-i", filePath+"/"+ciphertextFileName, "-o", filePath+"/"+plaintextFileName)
	command.Stdout = &bytes.Buffer{}
	command.Stderr = &bytes.Buffer{}

	err := command.Run()
	if err != nil {
		fmt.Println(err)
		fmt.Println(command.Stderr.(*bytes.Buffer).String())
		return err
	}
	fmt.Println(command.Stdout.(*bytes.Buffer).String())
	return nil
}

func getPlainText(plaintextFileName string) ([]byte, error) {
	filePath := "." //os.Executable()

	//file, err := os.OpenFile(filePath+"/"+plaintextFileName, os.O_WRONLY|os.O_CREATE, 0666)
	data, err := ioutil.ReadFile(filePath + "/" + plaintextFileName)
	if err != nil {
		fmt.Println(err)
		return nil, err
	}

	return []byte(string(data)), nil

}

func KeyGenPK(userName string) error {
	command := exec.Command("oabe_keygen", "-s", "PK", "-i", userName)
	command.Stdout = &bytes.Buffer{}
	command.Stderr = &bytes.Buffer{}

	err := command.Run()
	if err != nil {
		fmt.Println(err)
		fmt.Println(command.Stderr.(*bytes.Buffer).String())
		return err
	}
	fmt.Println(command.Stdout.(*bytes.Buffer).String())
	return nil
}

func EncryptPK(data []byte, senderName string, receiverName string, inputFileName string, outputFileName string) error {
	//write data into file
	filePath := "." //os.Executable()

	_, err := os.Stat(filePath + "/" + inputFileName)
	if !os.IsNotExist(err) {
		err := os.Remove(filePath + "/" + inputFileName)
		if err != nil {
			fmt.Println(err)
			return err
		}
	}
	file, err := os.Create(filePath + "/" + inputFileName)
	if err != nil {
		fmt.Println(err)
		return err
	}
	file, err = os.OpenFile(filePath+"/"+inputFileName, os.O_WRONLY|os.O_CREATE, 0666)
	//close file
	defer func(file *os.File) {
		err := file.Close()
		if err != nil {
			fmt.Println(err)
		}
	}(file)
	//use *Writer with cache to write into file
	write := bufio.NewWriter(file)
	writeString, err := write.WriteString(string(data))
	if err != nil {
		fmt.Print("number of bytes written to file:")
		fmt.Println(writeString)
		fmt.Println(err)
		return err
	}

	//write
	err = write.Flush()
	if err != nil {
		fmt.Println(err)
		return err
	}

	command := exec.Command("oabe_enc", "-s", "PK", "-e", senderName, "-r", receiverName, "-i", inputFileName, "-o", outputFileName)
	command.Stdout = &bytes.Buffer{}
	command.Stderr = &bytes.Buffer{}

	err = command.Run()
	if err != nil {
		fmt.Println(err)
		fmt.Println(command.Stderr.(*bytes.Buffer).String())
		return err
	}
	fmt.Println(command.Stdout.(*bytes.Buffer).String())
	return nil
}

func DecryptPK(ciphertextFileName string, senderName string, receiverName string, plaintextFileName string) error {
	//write data into file
	filePath := "." //os.Executable()

	command := exec.Command("oabe_dec", "-s", "PK", "-e", senderName, "-r", receiverName, "-i", filePath+"/"+ciphertextFileName, "-o", filePath+"/"+plaintextFileName)
	command.Stdout = &bytes.Buffer{}
	command.Stderr = &bytes.Buffer{}

	err := command.Run()
	if err != nil {
		fmt.Println(err)
		fmt.Println(command.Stderr.(*bytes.Buffer).String())
		return err
	}
	fmt.Println(command.Stdout.(*bytes.Buffer).String())
	return nil
}
