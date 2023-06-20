package com.ischoolbar.programmer.service.admin.impl;

import java.util.Date;
import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.ischoolbar.programmer.dao.admin.LogDao;
import com.ischoolbar.programmer.entity.amdin.Log;
import com.ischoolbar.programmer.service.admin.LogService;

@Service
public class LogServiceImpl implements LogService {

	@Autowired
	private LogDao logDao;
	
	@Override
	public int add(Log log) {
		// TODO Auto-generated method stub
		return logDao.add(log);
	}

	@Override
	public void add(String content) {
		// TODO Auto-generated method stub
		Log log = new Log();
		log.setContent(content);
		log.setCreateTime(new Date());
		logDao.add(content);
	}

	@Override
	public List<Log> findList(Map<String, Object> queryMap) {
		// TODO Auto-generated method stub
		return logDao.findList(queryMap);
	}

	@Override
	public int getTotal(Map<String, Object> queryMap) {
		// TODO Auto-generated method stub
		return logDao.getTotal(queryMap);
	}

	@Override
	public int delete(String ids) {
		// TODO Auto-generated method stub
		return logDao.delete(ids);
	}

}
