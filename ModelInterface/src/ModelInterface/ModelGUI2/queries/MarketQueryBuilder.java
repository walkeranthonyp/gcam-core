package ModelInterface.ModelGUI2.queries;

import ModelInterface.ModelGUI2.xmldb.XMLDB;
import ModelInterface.common.DataPair;

import javax.swing.JList;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.ListSelectionModel;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Iterator;
import java.util.Vector;
import java.util.LinkedHashMap;
import java.util.HashMap;
import java.util.TreeMap;
import java.util.EventListener;

import com.sleepycat.dbxml.XmlResults;
import com.sleepycat.dbxml.XmlValue;
import com.sleepycat.dbxml.XmlException;

public class MarketQueryBuilder extends QueryBuilder {
	public static Map varList;
	protected Map goodList;
	public static String xmlName = "marketQuery";
	public MarketQueryBuilder(QueryGenerator qgIn) {
		super(qgIn);
		varList = new LinkedHashMap();
		varList.put("price", new Boolean(false));
		varList.put("demand", new Boolean(false));
		varList.put("supply", new Boolean(false));
		goodList = null;
	}
	public EventListener getListSelectionListener(final JComponentAdapter list, final JButton nextButton, final JButton cancelButton) {
		queryFunctions.removeAllElements();
		queryFunctions.add("distinct-values");
		queryFilter = "/scenario/world/Marketplace/";
		return (new ListSelectionListener() {
			public void valueChanged(ListSelectionEvent e) {
				int[] selectedInd = list.getSelectedRows();
				if(selectedInd.length == 0 && qg.currSel != 0) {
					nextButton.setEnabled(false);
					cancelButton.setText(" Cancel "/*cancelTitle*/);
				} else if(qg.currSel == 1 || qg.currSel == 2) {
					nextButton.setEnabled(true);
				} else if(qg.currSel == 3) {
					nextButton.setEnabled(false);
					cancelButton.setText("Finished");
				}
					/*
				 else if((qg.isSumable && (selectedInd[0] == 0 || selectedInd[0] == 1)) || selectedInd.length > 1
					|| ((String)list.getSelectedValues()[0]).startsWith("Group:")) {
					nextButton.setEnabled(false);
					cancelButton.setText("Finished");
				} else if(qg.currSel != 3){
					nextButton.setEnabled(true);
					cancelButton.setText(" Cancel "/*cancelTitle/);
				}
			*/
			}
		});
	}
	public JComponentAdapter doNext(JComponentAdapter list, JLabel label) {
		updateSelected(list);
		if(qg.currSel == 3) {
			for(Iterator it = varList.entrySet().iterator(); it.hasNext(); ) {
				Map.Entry me = (Map.Entry)it.next();
				if(((Boolean)me.getValue()).booleanValue()) {
					qg.var = (String)me.getKey();
					//System.out.println("var is "+var);
					qg.isSumable = qg.sumableList.contains(qg.var);
					/*
					if(isSumable) {
						System.out.println("it does contain it");
					} else {
						System.out.println("doesn't contain it");
					}
					*/
				}
			}
		}
		return updateList(list, label);
	}
	public JComponentAdapter doBack(JComponentAdapter list, JLabel label) {
		return updateList(list, label);
	}
	public void doFinish(JComponentAdapter list) {
		++qg.currSel;
		updateSelected(list);
		--qg.currSel;
		createXPath();
		qg.levelValues = list.getSelectedValues();
		queryFunctions = null;
		queryFilter = null;
	}
	public boolean isAtEnd() {
		return qg.currSel == 3;
	}
	public JComponentAdapter updateList(JComponentAdapter list, JLabel label) {
		Map temp = null;
		switch(qg.currSel) {
			case 2: {
					list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
					temp = varList;
					label.setText("Select Variable:");
					break;
			}
			case 3: {
					list.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
					if(goodList == null) {
						goodList = createList("market/MarketGoodOrFuel/text()", false);
						goodList.putAll(createList("market/group/@name", true));
					}
					temp = goodList;
					label.setText("Select Good/Fuel:");
					break;
			}
			default: System.out.println("Error currSel: "+qg.currSel);
		}
		Vector tempVector = new Vector();
		String[] currKeys = (String[])temp.keySet().toArray(new String[0]);
		((JList)list.getModel()).setListData(currKeys);
		// check the maps to see which ones are true and add it to the list of selected
		for (int i = 0; i < currKeys.length; ++i) {
			if (((Boolean)temp.get(currKeys[i])).booleanValue()) {
				tempVector.addElement(new Integer(i));
			}
		}
		int[] selected = new int[tempVector.size()];
		for (int i = 0; i < selected.length; i++) {
			selected[i] = ((Integer)tempVector.get(i)).intValue();
		}
		temp = null;
		tempVector = null;
		list.setSelectedRows(selected);
		return list;
	}
	public void updateSelected(JComponentAdapter list) {
		Object[] selectedKeys = list.getSelectedValues();
		Map selected = null;
		switch(qg.currSel) { case 2: {
					return;
			}
			case 3: {
					selected = varList;
					break;
			}
			case 4: {
					selected = goodList;
					break;
			}
			default: System.out.println("Error currSel: "+qg.currSel);
		}
		for(Iterator it = selected.entrySet().iterator(); it.hasNext(); ) {
			((Map.Entry)it.next()).setValue(new Boolean(false));
		}
		for(int i = 0; i < selectedKeys.length; ++i) {
			selected.put(selectedKeys[i], new Boolean(true));
		}
	}
	private String expandGroupName(String gName) {
		StringBuffer ret = new StringBuffer();
		XmlResults res = XMLDB.getInstance().createQuery(queryFilter+"market/[child::group[@name='"+gName+"']]/@name",
				queryFunctions, null, null);
		try {
			while(res.hasNext()) {
				ret.append("(child::text()='").append(res.next().asString()).append("') or ");
			}
		} catch(XmlException e) {
			e.printStackTrace();
		}
		ret.delete(ret.length()-4, ret.length());
		XMLDB.getInstance().printLockStats("expandGroupName");
		return ret.toString();
	}
	private void createXPath() {
		qg.xPath = createListPath(0);
		qg.nodeLevel = new DataPair<String, String>("market", "name");
		// default axis1Name to nodeLevel
		qg.axis1Name = qg.nodeLevel.getKey();
		qg.yearLevel = new DataPair<String, String>("market", "year");
		qg.axis2Name = "Year";
		qg.group = true;
	}
	public String createListPath(int level) {
		StringBuffer ret = new StringBuffer("Marketplace/market");
		boolean added = false;
		for(Iterator it = goodList.entrySet().iterator(); it.hasNext(); ) {
			Map.Entry me = (Map.Entry)it.next();
			if(((Boolean)me.getValue()).booleanValue()) {
				if(!added) {
					ret.append("[ child::MarketGoodOrFuel[ ");
					added = true;
				} else {
					ret.append(" or ");
				}
				if(((String)me.getKey()).startsWith("Group:")) {
					ret.append(expandGroupName(((String)me.getKey()).substring(7)));
				} else {
					ret.append("(child::text()='"+me.getKey()+"')");
				}
			}
		}
		if(added) {
			ret.append(" ] ]/");
		} else {
			ret.append("/");
		}
		ret.append(qg.var).append("/node()");
		//ret += "period/"+var+"/node()";
		System.out.println("The xpath is: "+ret.toString());
		return ret.toString();
	}
	private Map createList(String path, boolean isGroupNames) {
		System.out.println("Query path: "+path);
		LinkedHashMap ret = new LinkedHashMap();
		/*
		if(!isGroupNames && qg.isSumable) {
			ret.put("Sum All", new Boolean(false));
			ret.put("Group All", new Boolean(false));
		}
		*/
		XmlResults res = XMLDB.getInstance().createQuery(queryFilter+path, queryFunctions, null, null);
		try {
			while(res.hasNext()) {
				if(!isGroupNames) {
					ret.put(res.next().asString(), new Boolean(false));
				} else { 
					ret.put("Group: "+res.next().asString(), new Boolean(false));
				}
			}
		} catch(XmlException e) {
			e.printStackTrace();
		}
		res.delete();
		XMLDB.getInstance().printLockStats("createList");
		return ret;
	}
	public String getCompleteXPath(Object[] regions) {
		StringBuffer ins = new StringBuffer();
		boolean added = false;
		if(((String)regions[regions.length-1]).equals("Global")) {
			return qg.xPath;
		}
		for(int i = 0; i < regions.length; ++i) {
			if(!added) {
				ins.append(" and ContainedRegion[");
				added = true;
			} else {
				ins.append(" or ");
			}
			ins.append("(child::text() = '").append(regions[i]).append("')");
		}
		if(added) {
			ins.append(" ] ]/");
			String[] spStr = qg.xPath.split("\\]/");
			return ins.insert(0, spStr[0]).append(spStr[1]).toString();
		} else {
			return qg.xPath;
		}
	}
	public Object[] extractAxisInfo(XmlValue n, Map filterMaps) throws Exception {
		Vector ret = new Vector(2, 0);
		XmlValue nBefore;
		do {
			if(n.getNodeName().equals(qg.nodeLevel.getKey())) {
				if(qg.nodeLevel.getValue() == null) {
					ret.add(XMLDB.getAttr(n, "name"));
				} else {
					ret.add(XMLDB.getAttr(n, qg.nodeLevel.getValue()));
				}
			if(n.getNodeName().equals(qg.yearLevel.getKey())) {
				if(qg.yearLevel.getValue() == null) {
					ret.add(0, XMLDB.getAttr(n, "year"));
				} else {
					ret.add(XMLDB.getAttr(n, qg.yearLevel.getValue()));
				}
			}

			} else if(XMLDB.hasAttr(n)) {
				Map tempFilter;
				if (filterMaps.containsKey(n.getNodeName())) {
					tempFilter = (HashMap)filterMaps.get(n.getNodeName());
				} else {
					tempFilter = new HashMap();
				}
				String attr = XMLDB.getAttr(n);
				if (!tempFilter.containsKey(attr)) {
					tempFilter.put(attr, new Boolean(true));
					filterMaps.put(n.getNodeName(), tempFilter);
				}
			}
			nBefore = n;
			n = n.getParentNode();
			nBefore.delete();
		} while(n.getNodeType() != XmlValue.DOCUMENT_NODE); 
		n.delete();
		XMLDB.getInstance().printLockStats("SupplyDemandQueryBuilder.getRegionAndYearFromNode");
		return ret.toArray();
	}
	public Map addToDataTree(XmlValue currNode, Map dataTree) throws Exception {
		if (currNode.getNodeType() == XmlValue.DOCUMENT_NODE) {
			currNode.delete();
			return dataTree;
		}
		Map tempMap = addToDataTree(currNode.getParentNode(), dataTree);
		// used to combine sectors and subsectors when possible to avoid large amounts of sparse tables
		if(XMLDB.hasAttr(currNode) && !currNode.getNodeName().equals(qg.nodeLevel.getKey()) 
				&& !currNode.getNodeName().equals(qg.yearLevel.getKey())) {
			String attr = XMLDB.getAllAttr(currNode);
			// check for rewrites
			if(qg.labelRewriteMap != null && qg.labelRewriteMap.containsKey(currNode.getNodeName())) {
				Map<String, String> currRewriteMap = qg.labelRewriteMap.get(currNode.getNodeName());
				if(currRewriteMap.containsKey(attr)) {
					attr = currRewriteMap.get(attr);
				}
			}
			attr = currNode.getNodeName()+"@"+attr;
			if(!tempMap.containsKey(attr)) {
				tempMap.put(attr, new TreeMap());
			}
			currNode.delete();
			return (Map)tempMap.get(attr);
		} 
		currNode.delete();
		return tempMap;
	}
	public String getXMLName() {
		return xmlName;
	}
	public List<String> getDefaultCollpaseList() {
		List<String> defaultCollapse = new Vector<String>(3);
		defaultCollapse.add("price");
		defaultCollapse.add("supply");
		defaultCollapse.add("demand");
		return defaultCollapse;
	}
}
