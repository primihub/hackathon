package gin

import (
	"bytes"
	"encoding/json"
	"io/ioutil"
	"net/http"
	"runtime/debug"
	"strings"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/privacy-protection/demo-api/common"
	"github.com/privacy-protection/demo-api/seelog"
)

func panicRecover() gin.HandlerFunc {
	return func(c *gin.Context) {
		defer func() {
			if err := recover(); err != nil {
				stack := debug.Stack()
				seelog.Errorf("panic stack: %s", string(stack))
				result := make(map[string]interface{})
				result["code"] = common.ErrorPanic
				result["message"] = "panic"
				b, err := json.Marshal(result)
				if err != nil {
					seelog.Errorf("marshal Response error, %v", err)
				}
				c.JSON(http.StatusOK, string(b))
				return
			}
		}()
		c.Next()
	}
}

func infoLog() gin.HandlerFunc {
	return func(c *gin.Context) {
		start := time.Now()

		body, _ := ioutil.ReadAll(c.Request.Body)
		c.Request.Body.Close()
		c.Request.Body = ioutil.NopCloser(bytes.NewBuffer(body))
		seelog.Infof("Request url: %s %v, body: %s", c.Request.Method, c.Request.URL, string(body))
		blw := bodyLogWriter{BodyBuf: bytes.NewBufferString(""), ResponseWriter: c.Writer}
		c.Writer = blw
		c.Next()
		respBody := strings.Trim(blw.BodyBuf.String(), "\n")
		seelog.Infof("Response url: %s %v, body: %s", c.Request.Method, c.Request.URL, string(respBody))
		duration := float64(time.Since(start)) / float64(time.Millisecond)
		seelog.Infof("Time: %vms", duration)
	}
}

type bodyLogWriter struct {
	gin.ResponseWriter
	BodyBuf *bytes.Buffer
}

func (w bodyLogWriter) Write(b []byte) (int, error) {
	w.BodyBuf.Write(b)
	return w.ResponseWriter.Write(b)
}
