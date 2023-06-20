package crypto

import (
	"testing"

	"github.com/privacy-protection/demo-api/conf"
	"github.com/stretchr/testify/require"
)

func TestAES(t *testing.T) {
	conf.Conf = &conf.Config{CryptoConf: conf.CryptoConfig{AESKey: "1234567890123456"}}
	text := []byte("test")
	ciphertext, err := AESEncrypt(text)
	require.NoError(t, err)
	b, err := AESDecrypt(ciphertext)
	require.NoError(t, err)
	require.Equal(t, text, b)
}
