package fle

import (
	"bytes"
	"testing"

	"github.com/stretchr/testify/require"
)

func TestSetup(t *testing.T) {
	err := Setup("local")
	require.NoError(t, err)
}

func TestDecrypt(t *testing.T) {
	err := Setup("local")
	require.NoError(t, err)

	err = KeyGen("(CompanyA and Male)", "local", "test1")
	require.NoError(t, err)

	data := []byte("hello world!")
	err = Encrypt(data, []string{"CompanyA", "Leader", "Male"}, "test1", "local", "input", "out")
	require.NoError(t, err)

	err = Decrypt("out.kpabe", "test1", "local", "plaintextFile1")
	require.NoError(t, err)

	plaintext, err := getPlainText("plaintextFile1")
	require.NoError(t, err)
	require.True(t, bytes.Equal(data, plaintext))
}

func TestInvalidDecrypt(t *testing.T) {
	err := Setup("local")
	require.NoError(t, err)

	err = KeyGen("(CompanyA and Male)", "local", "test1")
	require.NoError(t, err)

	data := []byte("hello world!")
	err = Encrypt(data, []string{"CompanyB", "Leader", "FeMale"}, "test1", "local", "input", "out")
	require.NoError(t, err)

	err = Decrypt("out.kpabe", "test1", "local", "plaintextFile1")
	require.Error(t, err)
}
