package com.ischoolbar.programmer.service.admin;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Service;

import com.ischoolbar.programmer.entity.amdin.User;

/**
 * user用户service
 * @author lenovo
 *
 */
@Service
public interface UserService {
	public User findByUsername(String username);
	public int add(User user);
	public int edit(User user);
	public int delete(String ids);
	public List<User> findList(Map<String, Object> quertMap);
	public int getTotal(Map<String, Object> queryMap);
	public int editPassword(User user);

}
