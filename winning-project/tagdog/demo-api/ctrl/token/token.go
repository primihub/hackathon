package token

import (
	"fmt"
	"time"

	"github.com/dgrijalva/jwt-go"
	"github.com/privacy-protection/demo-api/conf"
)

// UserClaims 用户token信息
type UserClaims struct {
	Username string `json:"username"`
	jwt.StandardClaims
}

// GenerateToken 生成token
func GenerateToken(username string) (string, error) {
	user := &UserClaims{Username: username}
	user.IssuedAt = time.Now().Unix()
	user.ExpiresAt = time.Now().Add(time.Second * time.Duration(conf.Conf.ServerConf.TokenExpireTime)).Unix()
	token := jwt.NewWithClaims(jwt.SigningMethodHS256, user)
	signedToken, err := token.SignedString([]byte(conf.Conf.ServerConf.TokenSecret))
	if err != nil {
		return "", fmt.Errorf("sign token error, %v", err)
	}
	return signedToken, nil
}

// VerifyToken 验证token
func VerifyToken(token string) (string, error) {
	t, err := jwt.ParseWithClaims(token, &UserClaims{}, func(t *jwt.Token) (interface{}, error) {
		return []byte(conf.Conf.ServerConf.TokenSecret), nil
	})
	if err != nil {
		return "", fmt.Errorf("parse token error, %v", err)
	}
	if err := t.Claims.Valid(); err != nil {
		return "", fmt.Errorf("invalid token, error: %v", err)
	}
	user, ok := t.Claims.(*UserClaims)
	if !ok {
		return "", fmt.Errorf("invalid token type: %T", t.Claims)
	}
	return user.Username, nil
}
