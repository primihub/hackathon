package com.ischoolbar.programmer.dao.admin;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.ischoolbar.programmer.entity.amdin.Log;

/**
 *日志管理Dao
 * @author lenovo
 *
 */
@Repository
public interface LogDao {

	public int add(Log log);
	public void add(String content);
	public List<Log> findList(Map<String, Object> queryMap);
	public int getTotal(Map<String, Object> queryMap);
	public int delete(String ids);
}
