package ole

import (
	"bytes"
	"github.com/stretchr/testify/require"
	"testing"
)

func TestDecrypt(t *testing.T) {
	err := Setup("local")
	require.NoError(t, err)

	err = KeyGen([]string{"CompanyA", "Leader", "Male"}, "test1", "local")
	require.NoError(t, err)

	data := []byte("hello world!")
	err = Encrypt(data, "(CompanyA and Male)",  "local", "input", "out")
	require.NoError(t, err)

	err = Decrypt("out.cpabe", "test1", "local", "plaintextFile1")
	require.NoError(t, err)

	plaintext, err := getPlainText("plaintextFile1")
	require.NoError(t, err)
	require.True(t, bytes.Equal(data, plaintext))
}

func TestInvalidDecrypt(t *testing.T) {
	err := Setup("local")
	require.NoError(t, err)

	err = KeyGen([]string{"CompanyB", "Leader", "FeMale"}, "test2", "local")
	require.NoError(t, err)

	data := []byte("hello world!")
	err = Encrypt(data, "(CompanyA and Male)",  "local", "input", "out")
	require.NoError(t, err)

	err = Decrypt("out.cpabe", "test2", "local", "plaintextFile1")
	require.Error(t, err)
}

func TestDecryptPK(t *testing.T) {
	err := KeyGenPK("Alice")
	require.NoError(t, err)

	err = KeyGenPK("Bob")
	require.NoError(t, err)

	data := []byte("hello, world!")
	err = EncryptPK(data, "Alice", "Bob", "input.txt", "input.pkenc")
	require.NoError(t, err)

	err = DecryptPK("input.pkenc", "Alice", "Bob", "plaintextFile_PK")
	require.NoError(t, err)

	plaintext, err := getPlainText("plaintextFile_PK")
	require.NoError(t, err)
	require.True(t, bytes.Equal(data, plaintext))
}

func TestInvalidDecryptPK(t *testing.T) {
	err := KeyGenPK("Alice")
	require.NoError(t, err)

	err = KeyGenPK("Bob")
	require.NoError(t, err)

	err = KeyGenPK("Carol")
	require.NoError(t, err)

	data := []byte("hello, world!")
	err = EncryptPK(data, "Alice", "Bob", "input.txt", "input.pkenc")
	require.NoError(t, err)

	err = DecryptPK("input.pkenc", "Alice", "Carol", "plaintextFile_PK")
	require.Error(t, err)
}
