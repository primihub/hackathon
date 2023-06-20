package com.ischoolbar.programmer.dao.admin;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.ischoolbar.programmer.entity.amdin.Menu;

/**
 * 菜单管理Dao
 * @author lenovo
 *
 */
@Repository
public interface MenuDao {
	public int add(Menu menu);	
	public List<Menu> findList(Map<String, Object> queryMap);
	public List<Menu> findTopList();
	public int getTotal(Map<String, Object> queryMap);
	public int edit(Menu menu);
	public List<Menu> findChildernList(Long parentId);
	public int delete(Long id);
	public List<Menu> findListByIds(String ids);

}
