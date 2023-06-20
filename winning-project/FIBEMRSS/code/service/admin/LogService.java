package com.ischoolbar.programmer.service.admin;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Service;
import com.ischoolbar.programmer.entity.amdin.Log;

/**
 *日志service接口
 * @author lenovo
 *
 */
@Service
public interface LogService {
	public int add(Log log);
	public void add(String content);
	public List<Log> findList(Map<String, Object> queryMap);
	public int getTotal(Map<String, Object> queryMap);
	public int delete(String ids);
}
